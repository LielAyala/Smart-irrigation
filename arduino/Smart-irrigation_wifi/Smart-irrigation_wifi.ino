#include <DHT.h> // ספרייה לניהול חיישני טמפרטורה ולחות
#include <ArduinoJson.h> // ספרייה לעבודה עם JSON (פרסור והמרת נתונים)

// הגדרת חיישן טמפרטורה ---------------------------------------------
#define dhtPIN 16 // הפין שאליו מחובר חיישן הטמפרטורה
#define DHTTYPE DHT11 // סוג החיישן DHT11

DHT dht(dhtPIN, DHTTYPE); // יצירת אובייקט חיישן DHT לקריאה מהחיישן

// הגדרת חיישני אור ולחות קרקע --------------------------------------
#define lightSensor 32 // הפין שאליו מחובר חיישן האור
#define Soil_moisture_Sensor 35 // הפין שאליו מחובר חיישן לחות הקרקע

// הגדרת פינים לשליטה במשאבה ----------------------------------------
#define B1a 19 // פין שליטה במשאבה
#define B1b 18 // פין שליטה במשאבה

// משתנים גלובליים ---------------------------------------------------
DynamicJsonDocument doc(1024); // משתנה לאחסון נתוני JSON בגודל 1024 בתים
int desiredMoisture = 50; // ערך ברירת מחדל של לחות קרקע

// הגדרת מצבי מערכת (State Machine) ----------------------------------
#define TEMP_MODE 61 // מצב מדידת טמפרטורה
#define SOIL_MOISTURE_MODE 62 // מצב מדידת לחות קרקע
#define SATURDAY_MODE 63 // מצב הפעלה בשבת
#define MANUAL_MODE 64 // מצב ידני

int CurrentStatus; // משתנה שמחזיק את מצב המערכת הנוכחי
unsigned long statusCheckTime; // משתנה לשמירת הזמן האחרון לבדיקה
unsigned long DataPullTime; // משתנה לשמירת זמן עדכון נתונים
unsigned long activationTime; // משתנה לשמירת זמן ההפעלה האחרון
unsigned long lastTimeOn; // משתנה לשמירת הזמן האחרון שהמשאבה הופעלה

float CurrentTemp; // משתנה לטמפרטורה נוכחית
int light; // משתנה לעוצמת אור
int minutes = (1000 * 60); // חישוב של דקה במילישניות
float temp; // משתנה לערך טמפרטורה
int minT, maxT; // משתנים לטווחי זמן הפעלה
bool isPumpON; // משתנה לבדיקת מצב המשאבה
int countON = 0; // מונה הפעלות משאבה

void setup() {
  dht.begin(); // הפעלת חיישן הטמפרטורה
  Serial.begin(115200); // הפעלת תקשורת סריאלית עם מהירות 115200
  WiFi_Setup(); // אתחול חיבור ה-WiFi

  dcMotorSetup(); // הגדרת מצב עבודה של הפינים לשליטה במשאבה
  delay(100); // השהיה של 100 מילישניות

  isPumpON = true; // הגדרת המשאבה כפעילה
  statusCheckTime = millis(); // שמירת הזמן הנוכחי לבדיקה עתידית
}

void loop() {
  float t = dht.readTemperature(); // קריאת טמפרטורה מהחיישן
  int light = analogRead(lightSensor); // קריאת עוצמת אור מהחיישן
  int soilMoisture = analogRead(Soil_moisture_Sensor); // קריאת לחות הקרקע מהחיישן

  sendData(t, light, soilMoisture); // שליחת הנתונים

  delay(2000); // השהיה של 2 שניות בין קריאות

  if (millis() - statusCheckTime > (10 * minutes)) { // בדיקה אם עברו 10 דקות מאז הבדיקה האחרונה
    CurrentStatus = GetState(); // קביעת מצב המערכת הנוכחי
    statusCheckTime = millis(); // עדכון זמן הבדיקה האחרון
  }

  // מעבר בין המצבים השונים של המערכת
  switch (CurrentStatus) {
    case TEMP_MODE: { // מצב מדידת טמפרטורה
    Serial.print(TEMP_MODE);
      CurrentTemp = dht.readTemperature(); // קריאת טמפרטורה נוכחית
      light = map(analogRead(lightSensor), 0, 4095, 0, 100); // המרת קריאת האור לטווח 0-100

      if ((millis() - DataPullTime) > (2 * minutes)) { // בדיקה אם עברו 2 דקות מאז עדכון הנתונים האחרון
        deserializeJson(doc, getJsonData("tempMode")); // קבלת נתוני JSON מהשרת
        temp = (float) doc["temp"]; // שליפת ערך הטמפרטורה מה-JSON
        minT = doc["minTime"].as<int>() * minutes; // שליפת זמן מינימלי
        maxT = doc["maxTime"].as<int>() * minutes; // שליפת זמן מקסימלי
        activationTime = doc["duration"].as<int>() * minutes; // שגיאה: המשתנה activationDuration לא הוגדר

        DataPullTime = millis(); // עדכון זמן משיכת הנתונים האחרון
      }

      if (CurrentTemp > temp) { // אם הטמפרטורה גבוהה מהערך הרצוי
        setOn(); // הפעלת המשאבה
        delay(maxT); // השהיית הפעלה לפי זמן מקסימלי
        Off(); // כיבוי המשאבה
      } else {
        setOn(); // הפעלת המשאבה
        delay(minT); // השהיית הפעלה לפי זמן מינימלי
        Off(); // כיבוי המשאבה
      }
      break;
    }

    case SOIL_MOISTURE_MODE: { // מצב מדידת לחות קרקע
    Serial.print(SOIL_MOISTURE_MODE);
      if ((millis() - DataPullTime) > (2 * minutes)) { // בדיקה אם עברו 2 דקות מאז עדכון הנתונים האחרון
        deserializeJson(doc, getJsonData("soilMoistureMode")); // קבלת נתוני JSON מהשרת
        int desiredMoisture = doc["moisture"]; // שליפת ערך הלחות הרצוי
        DataPullTime = millis(); // עדכון זמן משיכת הנתונים האחרון
      }

      if (soilMoisture < desiredMoisture - 10) { // אם הלחות מתחת לטווח הרצוי
        setOn(); // הפעלת המשאבה
      } else if (soilMoisture > desiredMoisture + 10) { // אם הלחות מעל הטווח הרצוי
        Off(); // כיבוי המשאבה
      }
      break;
    }

    case SATURDAY_MODE: { // מצב שבת
    Serial.print(SATURDAY_MODE);
      deserializeJson(doc, getJsonData("saturdayMode")); // קבלת נתוני JSON מהשרת
      int activationHour = doc["hour"]; // שליפת שעה להפעלה
      int activationDuration = doc["duration"].as<int>() * minutes; // שגיאה: אין התאמה בין טיפוסים

      if (shouldActivate(activationHour)) { // בדיקה אם צריך להפעיל את המערכת לפי שעה
        setOn(); // הפעלת המשאבה
        delay(activationDuration); // השהיית הפעלה לפי זמן מה-JSON
        Off(); // כיבוי המשאבה
      }
      break;
    }

    case MANUAL_MODE: { // מצב ידני
    Serial.print(MANUAL_MODE);
      deserializeJson(doc, getJsonData("manualMode")); // קבלת נתוני JSON מהשרת
      bool activatePump = doc["activate"]; // שליפת ערך הפעלת המשאבה
      delay(3000); // השהיה של 3 שניות

      if (activatePump) { // אם ההפעלה הידנית נדרשת
        setOn(); // הפעלת המשאבה
      } else {
        Off(); // כיבוי המשאבה
      }
      break;
    }
  }
}

// פונקציה לבדיקה אם יש להפעיל את המערכת לפי שעה
bool shouldActivate(int hour) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) { // אם לא ניתן לקבל את הזמן המקומי
    return false;
  }
  return (timeinfo.tm_hour == hour); // החזרת אמת אם השעה תואמת לשעת ההפעלה
}

// פונקציה לאתחול פינים של מנוע
void dcMotorSetup() {
    pinMode(B1a, OUTPUT);
    pinMode(B1b, OUTPUT);
}

// הפעלת המשאבה
void setOn() {
	  digitalWrite(B1a, LOW);
	  digitalWrite(B1b, HIGH);
}

// כיבוי המשאבה
void Off() {
    digitalWrite(B1a, HIGH);
    digitalWrite(B1b, HIGH);
}
