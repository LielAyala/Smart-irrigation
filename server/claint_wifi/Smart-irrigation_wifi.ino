#include <DHT.h>           // ספרייה לניהול חיישני טמפרטורה ולחות
#include <ArduinoJson.h>   // ספרייה לעבודה עם JSON (פרסור והמרת נתונים)

// הגדרת חיישן טמפרטורה ---------------------------------------------
#define dhtPIN 16           // הפין שאליו מחובר חיישן הטמפרטורה
#define DHTTYPE DHT11       // סוג החיישן DHT11

DHT dht(dhtPIN, DHTTYPE);  // יצירת אובייקט חיישן DHT לקריאה מהחיישן

// הגדרת חיישני אור ולחות קרקע --------------------------------------
#define lightSensor 32            // הפין שאליו מחובר חיישן האור
#define Soil_moisture_Sensor 35   // הפין שאליו מחובר חיישן לחות הקרקע

// הגדרת פינים לשליטה במשאבה ----------------------------------------
#define B1a 19  // פין שליטה במשאבה
#define B1b 18  // פין שליטה במשאבה

// משתנים גלובליים ---------------------------------------------------
DynamicJsonDocument doc(1024);  // משתנה לאחסון נתוני JSON בגודל 1024 בתים
int desiredMoisture = 50;       // ערך ברירת מחדל של לחות קרקע

// הגדרת מצבי מערכת (State Machine) ----------------------------------
#define TEMP_MODE 61            // מצב מדידת טמפרטורה
#define SOIL_MOISTURE_MODE 62   // מצב מדידת לחות קרקע
#define SATURDAY_MODE 63        // מצב הפעלה בשבת
#define MANUAL_MODE 64          // מצב ידני

int CurrentStatus;             // משתנה שמחזיק את מצב המערכת הנוכחי
unsigned long statusCheckTime; // משתנה לשמירת הזמן האחרון לבדיקה
unsigned long DataPullTime;    // משתנה לשמירת זמן עדכון נתונים
unsigned long activationTime;  // משתנה לשמירת זמן ההפעלה האחרון

float CurrentTemp;  // משתנה לטמפרטורה נוכחית
int light;         // משתנה לעוצמת אור
int minutes = (1000 * 60);  // חישוב של דקה במילישניות
float temp;        // משתנה לערך טמפרטורה
int minT, maxT;   // משתנים לטווחי זמן הפעלה
bool isPumpON;    // משתנה לבדיקת מצב המשאבה

void setup() {
    Serial.begin(115200);  // הפעלת תקשורת סריאלית
    dht.begin();           // הפעלת חיישן הטמפרטורה
    WiFi_Setup();         // התחברות לרשת WiFi

    dcMotorSetup();        // הגדרת מצב עבודה של המשאבה
    delay(100);            // השהיה קצרה לפני הפעלה

    isPumpON = false;  
    statusCheckTime = millis();  
}

void loop() {
    // קריאת נתונים מהחיישנים
    float t = dht.readTemperature();  
    int light = analogRead(lightSensor);  
    int soilMoisture = analogRead(Soil_moisture_Sensor);  

    // שליחת הנתונים לשרת
    sendData(t, light, soilMoisture, 1);  

    delay(2000);  // השהיה של 2 שניות בין הקריאות

    // כל 10 דקות לבדוק את מצב המערכת מהשרת
    if (millis() - statusCheckTime > (10 * minutes)) {  
        CurrentStatus = GetState();  
        statusCheckTime = millis();  
    }

    // ניהול מכונת המצבים
    switch (CurrentStatus) {
        case TEMP_MODE: {  // מצב מדידת טמפרטורה
            Serial.println("🌡️ מצב: מדידת טמפרטורה");
            CurrentTemp = dht.readTemperature();  

            if ((millis() - DataPullTime) > (2 * minutes)) {  
                deserializeJson(doc, getJsonData("tempMode"));  
                temp = (float) doc["temp"];  
                minT = doc["minTime"].as<int>() * minutes;  
                maxT = doc["maxTime"].as<int>() * minutes;  

                DataPullTime = millis();  
            }

            // אם חם יותר מהטמפרטורה הרצויה, השקיה ארוכה יותר
            if (CurrentTemp > temp) {  
                setOn();  
                delay(maxT);  
                Off();  
            } else {  
                setOn();  
                delay(minT);  
                Off();  
            }
            break;
        }

        case SOIL_MOISTURE_MODE: {  // מצב מדידת לחות קרקע
            Serial.println("💧 מצב: בדיקת לחות הקרקע");
            
            if ((millis() - DataPullTime) > (2 * minutes)) {  
                deserializeJson(doc, getJsonData("soilMoistureMode"));  
                desiredMoisture = doc["moisture"];  
                DataPullTime = millis();  
            }

            if (soilMoisture < desiredMoisture - 10) {  
                setOn();  
            } else if (soilMoisture > desiredMoisture + 10) {  
                Off();  
            }
            break;
        }

        case SATURDAY_MODE: {  // מצב שבת
            Serial.println("🕍 מצב: שבת");
            deserializeJson(doc, getJsonData("saturdayMode"));  
            int activationHour = doc["hour"];  

            if (shouldActivate(activationHour)) {  
                setOn();  
                delay(activationTime);  
                Off();  
            }
            break;
        }

        case MANUAL_MODE: {  // מצב ידני
            Serial.println("🛠️ מצב: ידני");
            deserializeJson(doc, getJsonData("manualMode"));  
            bool activatePump = doc["activate"];  

            if (activatePump) {  
                setOn();  
            } else {  
                Off();  
            }
            break;
        }
    }
}

// פונקציה לבדיקה אם יש להפעיל את המערכת לפי שעה
bool shouldActivate(int hour) {  
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {  
        return false;
    }
    return (timeinfo.tm_hour == hour);  
}

// הגדרת פינים למשאבה
void dcMotorSetup() {
    pinMode(B1a, OUTPUT);
    pinMode(B1b, OUTPUT);
}

// הפעלת המשאבה
void setOn() {
    Serial.println("🚰 הפעלת המשאבה");
    digitalWrite(B1a, LOW);
    digitalWrite(B1b, HIGH);
}

// כיבוי המשאבה
void Off() {
    Serial.println("🚱 כיבוי המשאבה");
    digitalWrite(B1a, HIGH);
    digitalWrite(B1b, HIGH);
}



// #include <DHT.h> // ספרייה לניהול חיישני טמפרטורה ולחות
// #include <ArduinoJson.h> // ספרייה לעבודה עם JSON (פרסור והמרת נתונים)

// // הגדרת חיישן טמפרטורה ---------------------------------------------
// #define dhtPIN 16 // הפין שאליו מחובר חיישן הטמפרטורה
// #define DHTTYPE DHT11 // סוג החיישן DHT11

// DHT dht(dhtPIN, DHTTYPE); // יצירת אובייקט חיישן DHT לקריאה מהחיישן

// // הגדרת חיישני אור ולחות קרקע --------------------------------------
// #define lightSensor 32 // הפין שאליו מחובר חיישן האור
// #define Soil_moisture_Sensor 35 // הפין שאליו מחובר חיישן לחות הקרקע

// // הגדרת פינים לשליטה במשאבה ----------------------------------------
// #define B1a 19 // פין שליטה במשאבה
// #define B1b 18 // פין שליטה במשאבה

// // משתנים גלובליים ---------------------------------------------------
// DynamicJsonDocument doc(1024); // משתנה לאחסון נתוני JSON בגודל 1024 בתים
// int desiredMoisture = 50; // ערך ברירת מחדל של לחות קרקע

// // הגדרת מצבי מערכת (State Machine) ----------------------------------
// #define TEMP_MODE 61 // מצב מדידת טמפרטורה
// #define SOIL_MOISTURE_MODE 62 // מצב מדידת לחות קרקע
// #define SATURDAY_MODE 63 // מצב הפעלה בשבת
// #define MANUAL_MODE 64 // מצב ידני

// int CurrentStatus; // משתנה שמחזיק את מצב המערכת הנוכחי
// unsigned long statusCheckTime; // משתנה לשמירת הזמן האחרון לבדיקה
// unsigned long DataPullTime; // משתנה לשמירת זמן עדכון נתונים
// unsigned long activationTime; // משתנה לשמירת זמן ההפעלה האחרון
// unsigned long lastTimeOn; // משתנה לשמירת הזמן האחרון שהמשאבה הופעלה

// float CurrentTemp; // משתנה לטמפרטורה נוכחית
// int light; // משתנה לעוצמת אור
// int minutes = (1000 * 60); // חישוב של דקה במילישניות
// float temp; // משתנה לערך טמפרטורה
// int minT, maxT; // משתנים לטווחי זמן הפעלה
// bool isPumpON; // משתנה לבדיקת מצב המשאבה
// int countON = 0; // מונה הפעלות משאבה

// void setup() {
//   dht.begin(); // הפעלת חיישן הטמפרטורה
//   Serial.begin(115200); // הפעלת תקשורת סריאלית עם מהירות 115200
//   WiFi_Setup(); // אתחול חיבור ה-WiFi

//   dcMotorSetup(); // הגדרת מצב עבודה של הפינים לשליטה במשאבה
//   delay(100); // השהיה של 100 מילישניות

//   isPumpON = true; // הגדרת המשאבה כפעילה
//   statusCheckTime = millis(); // שמירת הזמן הנוכחי לבדיקה עתידית
// }

// void loop() {
//   float t = dht.readTemperature(); // קריאת טמפרטורה מהחיישן
//   int light = analogRead(lightSensor); // קריאת עוצמת אור מהחיישן
//   int soilMoisture = analogRead(Soil_moisture_Sensor); // קריאת לחות הקרקע מהחיישן

//   sendData(t, light, soilMoisture); // שליחת הנתונים

//   delay(2000); // השהיה של 2 שניות בין קריאות

//   if (millis() - statusCheckTime > (10 * minutes)) { // בדיקה אם עברו 10 דקות מאז הבדיקה האחרונה
//     CurrentStatus = GetState(); // קביעת מצב המערכת הנוכחי
//     statusCheckTime = millis(); // עדכון זמן הבדיקה האחרון
//   }

//   // מעבר בין המצבים השונים של המערכת
//   switch (CurrentStatus) {
//     case TEMP_MODE: { // מצב מדידת טמפרטורה
//     Serial.print(TEMP_MODE);
//       CurrentTemp = dht.readTemperature(); // קריאת טמפרטורה נוכחית
//       light = map(analogRead(lightSensor), 0, 4095, 0, 100); // המרת קריאת האור לטווח 0-100

//       if ((millis() - DataPullTime) > (2 * minutes)) { // בדיקה אם עברו 2 דקות מאז עדכון הנתונים האחרון
//         deserializeJson(doc, getJsonData("tempMode")); // קבלת נתוני JSON מהשרת
//         temp = (float) doc["temp"]; // שליפת ערך הטמפרטורה מה-JSON
//         minT = doc["minTime"].as<int>() * minutes; // שליפת זמן מינימלי
//         maxT = doc["maxTime"].as<int>() * minutes; // שליפת זמן מקסימלי
//         activationTime = doc["duration"].as<int>() * minutes; // שגיאה: המשתנה activationDuration לא הוגדר

//         DataPullTime = millis(); // עדכון זמן משיכת הנתונים האחרון
//       }

//       if (CurrentTemp > temp) { // אם הטמפרטורה גבוהה מהערך הרצוי
//         setOn(); // הפעלת המשאבה
//         delay(maxT); // השהיית הפעלה לפי זמן מקסימלי
//         Off(); // כיבוי המשאבה
//       } else {
//         setOn(); // הפעלת המשאבה
//         delay(minT); // השהיית הפעלה לפי זמן מינימלי
//         Off(); // כיבוי המשאבה
//       }
//       break;
//     }

//     case SOIL_MOISTURE_MODE: { // מצב מדידת לחות קרקע
//     Serial.print(SOIL_MOISTURE_MODE);
//       if ((millis() - DataPullTime) > (2 * minutes)) { // בדיקה אם עברו 2 דקות מאז עדכון הנתונים האחרון
//         deserializeJson(doc, getJsonData("soilMoistureMode")); // קבלת נתוני JSON מהשרת
//         int desiredMoisture = doc["moisture"]; // שליפת ערך הלחות הרצוי
//         DataPullTime = millis(); // עדכון זמן משיכת הנתונים האחרון
//       }

//       if (soilMoisture < desiredMoisture - 10) { // אם הלחות מתחת לטווח הרצוי
//         setOn(); // הפעלת המשאבה
//       } else if (soilMoisture > desiredMoisture + 10) { // אם הלחות מעל הטווח הרצוי
//         Off(); // כיבוי המשאבה
//       }
//       break;
//     }

//     case SATURDAY_MODE: { // מצב שבת
//     Serial.print(SATURDAY_MODE);
//       deserializeJson(doc, getJsonData("saturdayMode")); // קבלת נתוני JSON מהשרת
//       int activationHour = doc["hour"]; // שליפת שעה להפעלה
//       int activationDuration = doc["duration"].as<int>() * minutes; // שגיאה: אין התאמה בין טיפוסים

//       if (shouldActivate(activationHour)) { // בדיקה אם צריך להפעיל את המערכת לפי שעה
//         setOn(); // הפעלת המשאבה
//         delay(activationDuration); // השהיית הפעלה לפי זמן מה-JSON
//         Off(); // כיבוי המשאבה
//       }
//       break;
//     }

//     case MANUAL_MODE: { // מצב ידני
//     Serial.print(MANUAL_MODE);
//       deserializeJson(doc, getJsonData("manualMode")); // קבלת נתוני JSON מהשרת
//       bool activatePump = doc["activate"]; // שליפת ערך הפעלת המשאבה
//       delay(3000); // השהיה של 3 שניות

//       if (activatePump) { // אם ההפעלה הידנית נדרשת
//         setOn(); // הפעלת המשאבה
//       } else {
//         Off(); // כיבוי המשאבה
//       }
//       break;
//     }
//   }
// }

// // פונקציה לבדיקה אם יש להפעיל את המערכת לפי שעה
// bool shouldActivate(int hour) {
//   struct tm timeinfo;
//   if (!getLocalTime(&timeinfo)) { // אם לא ניתן לקבל את הזמן המקומי
//     return false;
//   }
//   return (timeinfo.tm_hour == hour); // החזרת אמת אם השעה תואמת לשעת ההפעלה
// }

// // פונקציה לאתחול פינים של מנוע
// void dcMotorSetup() {
//     pinMode(B1a, OUTPUT);
//     pinMode(B1b, OUTPUT);
// }

// // הפעלת המשאבה
// void setOn() {
// 	  digitalWrite(B1a, LOW);
// 	  digitalWrite(B1b, HIGH);
// }

// // כיבוי המשאבה
// void Off() {
//     digitalWrite(B1a, HIGH);
//     digitalWrite(B1b, HIGH);
// }















// #include <DHT.h> //שימוש בספרייה של DHT של טמפרטורה 
// #include <ArdoinoJson>
// //פין טמפרטורה ---------------------------------------------
// #define dhtPIN 16// מקום אליו המכשיר מחובר 
// #define DHTTYPE DHT11 //סוג החיישן שאנחנו עובדים איתו 

// //----------------------------הגדרת חיישן טמפרטורה --------
// //פין וסוג  בדומה לקריאה למחלקה 
// DHT dht(dhtPIN, DHTTYPE); //המידע שיוחזר הוא מספר ממשי 
// //-----------------------------------------------------------

// //הגדרת חיישן אור ----------------------//לא מגדירים בSETUP 
// #define lightSensor 32

// //הגדרת חיישן לחות ----------------------//לא מגדירים בSETUP
// #define Soil_moisture_Sensor 35

// //פינים של משאבה -------------------------------------------
// #define B1a 19
// #define B1b 18

// // State Machine
// // -------------------------------------
// #define TEMP_MODE 61
// #define SOIL_MOISTURE_MODE 62
// #define SATURDAY_MODE 63
// #define MANUAL_MODE 64
// int CurrentStatus;
// unsigned long statusCheckTime;
// unsigned long DataPullTime;
// unsigned long activationTime;
// unsigned long lastTimeOn;
// // -------------------------------------

// float CurrentTemp;
// int light; 
// int minutes = (1000 * 60);
// float temp;
// int minT, maxT;
// bool isPumpON;
// int countON = 0;

// void setup(){
//   //הפעלת החיישן טמפרטורה
// 	dht.begin(); 
//   Serial.begin(115200);
//   WiFi_Setup();

//   dcMotorSetup();
//   delay(100);

// isPumpON = true;
// statusCheckTime = millis();

// }

// void loop(){
//   float t=dht.readTemperature();
//   //  Serial.print("temp=");//  Serial.print(t);
//   int light= analogRead(lightSensor);
//   // Serial.print("   light =");// Serial.print(light);
//   int Soil_moisture=analogRead(Soil_moisture_Sensor);
//   // Serial.print("   Soil_moisture =");// Serial.print(Soil_moisture);
//   sendData(t,light,Soil_moisture);// Serial.println("");
//   setOn();
//   delay(2000);

//   if(millis() - statusCheckTime > (10 * minutes)){
//     CurrentStatus = GetState();
//     statusCheckTime = millis();
//   }

//   switch(statusCheckTime){
//     case TEMP_MODE:
   
//     CurrentTemp = dht.readTemperature();
//     light = map(analogRead(lightSensor), 0, 4095, 0, 100);

//     if( (millis() - DataPullTime) > ( 2 * minutes) ){
//       deserializeJson(doc, getJsonData("tempMode"));
//       temp = (float) doc["temp"];
//       minT = doc["minTime"];
//       maxT = doc["maxTime"];
//       DataPullTime = millis();
//     }

//       if(light > 90 ) {
//         lastTimeOn=millis();
//         isPumpON = true;
//         setOn();
//         countON++;
//         activationTime=10000;
//         if(lastTimeOn==activationTime){
//            Off();
//         }

//       }

//       else if(light < 10 && countON == 2){
//         isPumpON = true;
//         setOn();
//         countON++;
//       } 
     

//     case SOIL_MOISTURE_MODE:

//   }
  

// }
// // מצב עבודה של הפינים
// void dcMotorSetup() {
//     pinMode(B1a, OUTPUT);
//     pinMode(B1b, OUTPUT);
// }
// //פונקציית הפעלה 
// void setOn() {
// 	  digitalWrite(B1a, LOW);
// 	  digitalWrite(B1b, HIGH);
// }
// //פונקציית כיבוי 
// void Off() {
//     digitalWrite(B1a, HIGH);
//     digitalWrite(B1b, HIGH);
// }
// //חיישן טמפרטורה 
// void temp(){
//   delay(5000);
//     // Reading temperature or humidity takes about 250 milliseconds!
//     // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
//     float h = dht.readHumidity();
//     // Read temperature as Celsius
//     float t = dht.readTemperature();
     
//     Serial.print("Temperature = ");
//     Serial.println(t);
//     Serial.print("Humidity = ");
//     Serial.println(h);
// }

// void State machine(){

//  enum state{
//    off,
//    on
//   }
//  state current_state=off;
//  bool isItKlik=false;

// }
