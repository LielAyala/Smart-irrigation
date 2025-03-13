#include <DHT.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

int GetState();
String getJsonData(String state);

// **הגדרת חיישנים ופינים**
#define dhtPIN 16  
#define DHTTYPE DHT11
DHT dht(dhtPIN, DHTTYPE);

#define lightSensor 32         
#define soilMoistureSensor 35  
#define pumpPin1 18  
#define pumpPin2 19  

// **מצבי מערכת**
#define TEMP_MODE 61
#define SOIL_MOISTURE_MODE 62
#define SATURDAY_MODE 63
#define MANUAL_MODE 64

// **משתנים גלובליים**
DynamicJsonDocument doc(1024);  
const int minutes = 60000;

float currentTemp;
int light, soilMoisture, minT, maxT;
bool isPumpON = false;
int countON = 0;

// **משתנים חדשים**
int waterUsageToday = 0;  
int treeID = 6;
unsigned long pumpStartTime = 0;
const int waterFlowPerMinute = 60;  

// **ניהול זמני מערכת**
int currentStatus;
unsigned long statusCheckTime;
unsigned long dataPullTime;
unsigned long activationTime;

// **NTP לקבלת שעה**
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200, 60000);

void setup() {
  pinMode(pumpPin1, OUTPUT);
  pinMode(pumpPin2, OUTPUT);
  Serial.begin(115200);
  WiFi_Setup();
  dht.begin();
  isPumpON = false;
  statusCheckTime = millis();
  digitalWrite(pumpPin1, HIGH);
  digitalWrite(pumpPin2, HIGH);
  timeClient.begin();
}

// **פונקציה להפעלת המשאבה ולמדידת כמות מים**
void activatePump(int duration) {
    pumpStartTime = millis();  
    digitalWrite(pumpPin1, HIGH);
    digitalWrite(pumpPin2, LOW);
    delay(duration * minutes);
    digitalWrite(pumpPin1, HIGH);
    digitalWrite(pumpPin2, HIGH);

    unsigned long pumpDuration = millis() - pumpStartTime;
    int waterUsed = (pumpDuration / 60000.0) * waterFlowPerMinute;
    waterUsageToday += waterUsed;

    Serial.print("מים שנצרכו בהפעלה זו: ");
    Serial.println(waterUsed);
    Serial.print("סך הכל מים שצרך העץ היום: ");
    Serial.println(waterUsageToday);

    sendWaterUsage(); // שולח את הנתונים לשרת
}


void loop() {
  light = map(analogRead(lightSensor), 0, 4095, 0, 100);
  float temp = dht.readTemperature();
  soilMoisture = map(analogRead(soilMoistureSensor), 0, 4095, 0, 100);

  Serial.print("Light = "); Serial.println(light);
  Serial.print("Temp = "); Serial.println(temp);
  Serial.print("Moisture = "); Serial.println(soilMoisture);
  
  timeClient.update();
  Serial.print("השעה: "); Serial.println(timeClient.getFormattedTime());

  if (millis() - statusCheckTime > (0 * minutes)) {
    currentStatus = GetState();
    statusCheckTime = millis();
  }
  delay(3000);

  // **מכונת מצבים**
  switch (currentStatus) {
    case TEMP_MODE:
    {
        Serial.println("נכנס למצב TEMP_MODE");
        currentTemp = dht.readTemperature();
        
        if ((millis() - dataPullTime) > (2 * minutes)) {
            deserializeJson(doc, getJsonData("tempMode"));
            minT = doc["minTime"];
            maxT = doc["maxTime"];
            dataPullTime = millis();
        }

        if (light > 90 || (light < 10 && countON == 2)) {
            isPumpON = true;
            countON = 0;
        }

        if (isPumpON && countON < 2) {
            activatePump(minT); 
            isPumpON = false;
            countON++;
        }
    }
    break;

    case SOIL_MOISTURE_MODE:
    {
        Serial.println("נכנס למצב SOIL_MOISTURE_MODE");
        soilMoisture = map(analogRead(soilMoistureSensor), 0, 4095, 0, 100);
        
        int maxMoist = max(soilMoisture, 40);
        int minMoist = min(soilMoisture, 20);
        
        if ((millis() - dataPullTime) > (2 * minutes)) {
            deserializeJson(doc, getJsonData("MoistureMode"));
            minT = doc["minTime"];
            maxT = doc["maxTime"];
            dataPullTime = millis();
        }
        
        if (soilMoisture > minMoist || soilMoisture < minMoist) {
            isPumpON = true;
        }
        
        if (isPumpON) {
            activatePump(maxT);
            isPumpON = false;
        }
    }
    break;

    case SATURDAY_MODE:
    {
        Serial.println("נכנס למצב SATURDAY_MODE");
        timeClient.update();
        
        int currentHour = timeClient.getHours();
        int currentMinute = timeClient.getMinutes();

        if ((millis() - dataPullTime) > (0 * minutes)) {
            deserializeJson(doc, getJsonData("SATURDAY_MODE"));
            int scheduledHour = doc["hour"];
            int scheduledMinute = doc["minute"];
            int duration = doc["duration"];
            
            dataPullTime = millis();
            activationTime = duration * minutes;

            if (currentHour == scheduledHour && currentMinute == scheduledMinute) {
                activatePump(duration);
            }
        }
    }
    break;

    case MANUAL_MODE:
    {
        Serial.println("נכנס למצב MANUAL_MODE");

        if ((millis() - dataPullTime) > (0 * minutes)) {
            deserializeJson(doc, getJsonData("MANUAL_MODE"));
            bool start = doc["enabled"];
            
            if (!start) {
                digitalWrite(pumpPin1, HIGH);
                digitalWrite(pumpPin2, HIGH);
                Serial.println("המשאבה כבויה");
            } else {
                Serial.println("המשאבה הופעלה ידנית");
                activatePump(1);
            }
        }
    }
    break;
  }
}
