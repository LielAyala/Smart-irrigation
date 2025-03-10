#include <DHT.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>  // ספרייה לתקשורת UDP

int GetState();
String getJsonData(String state);

// **הגדרת חיישנים ופינים**
#define dhtPIN 16  // חיישן טמפרטורה מחובר לפין 16
#define DHTTYPE DHT11
DHT dht(dhtPIN, DHTTYPE);

#define lightSensor 32         // חיישן אור מחובר לפין 32
#define soilMoistureSensor 35  // חיישן לחות קרקע מחובר לפין 35

#define pumpPin 19  // פין להפעלת המשאבה


// **מצבי מערכת - ערכים מספריים קבועים לכל מצב**
#define TEMP_MODE 61
#define SOIL_MOISTURE_MODE 62
#define SATURDAY_MODE 63
#define MANUAL_MODE 64

// **משתנים גלובליים**
DynamicJsonDocument doc(1024);  // משתנה לטיפול בנתוני JSON

const int minutes = 60000;  // הגדרת דקה במילישניות (1000ms * 60s)

float currentTemp;      // משתנה לאחסון טמפרטורה נוכחית
int light;              // משתנה לאחסון ערך חיישן האור
int soilMoisture;       // משתנה לאחסון ערך חיישן הלחות בקרקע
int minT, maxT;         // זמני הפעלה מוגדרים מראש
bool isPumpON = false;  // משתנה לבדיקה אם המשאבה פועלת
int countON = 0;

// **State Machine**
int currentStatus;              // משתנה שיחזיק את המצב הנוכחי של המערכת
unsigned long statusCheckTime;  // משתנה לניהול תזמון בדיקת מצב
unsigned long dataPullTime;     // משתנה לניהול תזמון שליחת נתונים לשרת
unsigned long activationTime;   // משתנה לשמירת זמן הפעלה אחרון

// // שרתי NTP לקבלת זמן
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200, 60000);  // שעון ישראל (UTC+2)

unsigned long lastSampleTime = 0;
const long sampleInterval =0;
// 3 * 60 * 60 * 1000;  // 3 שעות


void setup() {
  pinMode(pumpPin, OUTPUT);
  Serial.begin(115200);
  WiFi_Setup();
  dht.begin();
  isPumpON = false;
  statusCheckTime = millis();
  digitalWrite(pumpPin, HIGH);  // כיבוי המשאבה
  // הפעלת ה-NTP
  timeClient.begin();
}

void loop() {
  light = map(analogRead(lightSensor), 0, 4095, 0, 100);
  float temp = dht.readTemperature();
  soilMoisture = map(analogRead(soilMoistureSensor), 0, 4095, 0, 100);

  digitalWrite(pumpPin, HIGH);  // כיבוי המשאבה

  Serial.print("light = ");
  Serial.println(light);

  Serial.print("temp = ");
  Serial.println(temp);

  Serial.print("Moisture = ");
  Serial.println(soilMoisture);
  Serial.println("");
  delay(2000);
  Serial.println(currentStatus);
  Serial.println("");
  delay(2000);

  if (millis() - lastSampleTime >= sampleInterval) {
        lastSampleTime = millis();   
        sendSample("Temperature", temp);
        sendSample("Light", light);
        sendSample("SoilMoisture", soilMoisture);
    }
    delay(2000);


  //SendData(temp, light, soilMoisture);


  timeClient.update();
  Serial.print("השעה עכשיו: ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  delay(2000);


  if (millis() - statusCheckTime > (0 * minutes)) {
    currentStatus = GetState();
    statusCheckTime = millis();
  }

  switch (currentStatus) {
    case TEMP_MODE:
      {
        Serial.println("נכנס למצב TEMP_MODE ");
        currentTemp = dht.readTemperature();
        light = map(analogRead(lightSensor), 0, 4095, 0, 100);

        if ((millis() - dataPullTime) > (2 * minutes)) {
          deserializeJson(doc, getJsonData("tempMode"));
          temp = (float)doc["temp"];
          minT = doc["minTime"];
          maxT = doc["maxTime"];
          dataPullTime = millis();
        }

        if (light > 90) {
          isPumpON = true;
        } else if (light < 10 && countON == 2) {
          isPumpON = true;
          countON = 0;
        }

        if (isPumpON && temp < currentTemp && countON < 2 && light < 40) {
          digitalWrite(pumpPin, LOW);
          if (millis() - activationTime > (maxT * minutes)) {
            digitalWrite(pumpPin, HIGH);
            isPumpON = false;
            countON++;
            activationTime = millis();
          }
        } else if (isPumpON && countON < 2) {
          digitalWrite(pumpPin, LOW);
          if (millis() - activationTime > (minT * minutes)) {
            digitalWrite(pumpPin, HIGH);
            isPumpON = false;
            countON++;
            activationTime = millis();
          }
        }
      }
      break;
    case SOIL_MOISTURE_MODE:
      {
        Serial.println("נכנס למצב SOIL_MOISTURE_MODE ");
        soilMoisture = map(analogRead(soilMoistureSensor), 0, 4095, 0, 100);
        int maxMoist = max(soilMoisture, 40);
        int minMoist = min(soilMoisture, 20);

        if ((millis() - dataPullTime) > (2 * minutes)) {
          deserializeJson(doc, getJsonData("MoistureMode"));
          soilMoisture = (int)doc["moisture"];
          minT = doc["minTime"];
          maxT = doc["maxTime"];
          dataPullTime = millis();
        }

        if (soilMoisture > minMoist || soilMoisture < minMoist) {
          isPumpON = true;
        }

        if (isPumpON) {
          digitalWrite(pumpPin, LOW);
          if (millis() - activationTime > (maxT * minutes)) {
            digitalWrite(pumpPin, HIGH);
            isPumpON = false;
            activationTime = millis();
          }
        }
      }
      break;
    case SATURDAY_MODE:
      {
        Serial.println(" נכנס למצב SATURDAY_MODE ");

        // עדכון השעה הנוכחית מ-NTP
        timeClient.update();
        int currentHour = timeClient.getHours();
        int currentMinute = timeClient.getMinutes();

        // בדיקה אם צריך למשוך נתונים מהשרת (כל 0 דקות)
        if ((millis() - dataPullTime) > (0 * minutes)) {
          deserializeJson(doc, getJsonData("SATURDAY_MODE"));

          int scheduledHour = doc["hour"];      // שעה להפעלת המשאבה
          int scheduledMinute = doc["minute"];  // דקה להפעלת המשאבה
          int duration = doc["duration"];       // משך הפעלת המשאבה בדקות

          // שמירה של הנתונים כדי להשתמש בהם במעבר הבא בלולאה
          dataPullTime = millis();
          activationTime = duration * minutes;

          Serial.print(" שעה מתוזמנת להפעלה: ");
          Serial.print(scheduledHour);
          Serial.print(":");
          Serial.println(scheduledMinute);

          // שמירת הנתונים למשך ההשוואה עם הזמן הנוכחי
          if (currentHour == scheduledHour && currentMinute == scheduledMinute) {
            Serial.println(" מפעיל משאבה במצב שבת");
            digitalWrite(pumpPin, LOW);  // הפעלת המשאבה
            // המתנה לפי הזמן שהוגדר ואז כיבוי המשאבה
            delay(activationTime);
            digitalWrite(pumpPin, HIGH);  // כיבוי המשאבה
            Serial.println(" המשאבה כובתה לאחר הזמן שהוגדר");
          }
        }
      }
      break;
    case MANUAL_MODE:
      {
        Serial.println("MANUAL_MODE נכנסים למצב ידני  ");
        // בדיקה אם צריך למשוך נתונים מהשרת (כל 2 דקות)
        if ((millis() - dataPullTime) > (0 * minutes)) {
          deserializeJson(doc, getJsonData("MANUAL_MODE"));
          bool start = doc["enabled"];
          if (!start) {
            digitalWrite(pumpPin, HIGH);  // כיבוי המשאבה
            Serial.println(" המשאבה כבויה");
          } else {
            Serial.println(" המתנה של 3 שניות לפני הפעלת המשאבה...");
            delay(3000);                 // **עיכוב של 3 שניות לפני ההפעלה**
            digitalWrite(pumpPin, LOW);  // הפעלת המשאבה
            Serial.println(" המשאבה הופעלה!");
          }
        }
      }
      break;
  }
}