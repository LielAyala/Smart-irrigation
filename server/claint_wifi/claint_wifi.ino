#include <WiFi.h>         // ספרייה לניהול חיבור לרשת Wi-Fi
#include <WiFiClient.h>   // ספרייה לעבודה עם חיבור Wi-Fi
#include <HTTPClient.h>   // ספרייה לשליחת בקשות HTTP

// פרטי חיבור לרשת Wi-Fi
const char* ssid = "שם_הרשת_שלך";      // שם הרשת (SSID)
const char* password = "הסיסמה_שלך";  // סיסמת ה-WiFi

WiFiClient client; // יצירת אובייקט חיבור Wi-Fi

// כתובת השרת
const char* serverUrl = "http://192.168.1.160:3011/esp/sendData"; // כתובת ה-API לשליחת נתונים
const char* stateUrl = "http://192.168.1.160:3011/esp/state";      // כתובת ה-API לבדיקה תקופתית של מצב השרת

// פונקציה לאתחול חיבור לרשת ה-WiFi
void WiFi_Setup() {
  Serial.begin(115200);       // הפעלת תקשורת סריאלית
  delay(1000);                // השהיה קצרה לפני התחברות
  WiFi.begin(ssid, password); // התחברות לרשת

  Serial.println("מתחבר ל-Wi-Fi...");

  while (WiFi.status() != WL_CONNECTED) { // לולאה עד שהחיבור יצליח
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ חיבור לרשת הצליח!");
  Serial.print("📡 כתובת IP: ");
  Serial.println(WiFi.localIP()); // הצגת כתובת ה-IP שהוקצתה ל-ESP
}

// פונקציה לשליחת נתונים לשרת
void sendData(float temp, int light, int moisture, int plantNumber) {
  if (WiFi.status() == WL_CONNECTED) { // בדיקה אם יש חיבור לרשת
    HTTPClient http; // יצירת אובייקט HTTP

    http.begin(serverUrl); // התחלת בקשת HTTP
    http.addHeader("Content-Type", "application/json"); // הגדרת הכותרת כ-JSON

    // יצירת מחרוזת JSON עם הנתונים
    String jsonPayload = "{\"sensorName\": \"all\", \"plantNumber\": " + String(plantNumber) +
                         ", \"temp\": " + String(temp) + ", \"light\": " + String(light) + 
                         ", \"moisture\": " + String(moisture) + "}";

    Serial.println("📤 שולח נתונים לשרת: " + jsonPayload);

    int httpResponseCode = http.POST(jsonPayload); // שליחת הבקשה

    if (httpResponseCode > 0) { // אם הבקשה הצליחה
      Serial.print("✅ תגובת השרת: ");
      Serial.println(httpResponseCode);
      String response = http.getString(); // קבלת תשובת השרת
      Serial.println("📥 תשובת השרת: " + response);
    } else { // אם הבקשה נכשלה
      Serial.print("❌ שגיאה בשליחה! קוד שגיאה: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // סגירת החיבור
  } else {
    Serial.println("❌ אין חיבור לרשת, לא ניתן לשלוח נתונים!");
  }
}

// פונקציה לקבלת מצב המערכת מהשרת
int GetState() {
  if (WiFi.status() == WL_CONNECTED) { // בדיקה אם יש חיבור
    HTTPClient http; // יצירת אובייקט HTTP
    http.begin(stateUrl); // התחלת בקשת GET לשרת

    int httpCode = http.GET(); // שליחת בקשה לקבלת מצב המערכת

    if (httpCode == HTTP_CODE_OK) { // אם קיבלנו תשובה תקינה
      String Res = http.getString(); // קבלת תשובת השרת
      Serial.print("📥 מצב מערכת שהתקבל: ");
      Serial.println(Res);
      return Res.toInt(); // המרה למספר והחזרה
    } else { // אם הבקשה נכשלה
      Serial.println("❌ שגיאה בקבלת מצב המערכת!");
    }

    http.end(); // סגירת החיבור
  } else {
    Serial.println("❌ אין חיבור לרשת, לא ניתן לבדוק מצב!");
  }

  return -1; // במקרה של שגיאה נחזיר ערך ברירת מחדל
}



// #include <WiFi.h>
// #include <HTTPClient.h>

// // פרטי חיבור לרשת Wi-Fi
// const char* ssid = "שם_הרשת_שלך";
// const char* password = "הסיסמה_שלך";

// // כתובת השרת
// const char* serverUrl = "192.168.1.160";

// void setup() {
//   Serial.begin(115200); // הפעלת חיבור סריאלי
//   delay(1000);

//   // התחברות ל-Wi-Fi
//   WiFi.begin(ssid, password);
//   Serial.println("מתחבר ל-Wi-Fi...");
  
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
  
//   Serial.println("\nחיבור לרשת הצליח!");
//   Serial.print("כתובת IP: ");
//   Serial.println(WiFi.localIP());
// }

// void loop() {
//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http; // יצירת אובייקט HTTPClient
    
//     http.begin(serverUrl); // הגדרת כתובת השרת
//     http.addHeader("Content-Type", "application/json"); // הוספת כותרת
    
//     // שליחת בקשת POST עם נתונים לדוגמה
//     String jsonPayload = "{\"message\": \"Hello from ESP32!\"}";
//     int httpResponseCode = http.POST(jsonPayload);
    
//     if (httpResponseCode > 0) {
//       Serial.print("קוד תגובה: ");
//       Serial.println(httpResponseCode);
//       String response = http.getString();
//       Serial.println("תשובת השרת:");
//       Serial.println(response);
//     } else {
//       Serial.print("שגיאה בבקשה: ");
//       Serial.println(httpResponseCode);
//     }
    
//     http.end(); // סיום החיבור
//   } else {
//     Serial.println("איבדנו חיבור ל-Wi-Fi...");
//   }
  
//   delay(10000); // השהיה של 10 שניות בין כל בקשה
// }










//#include <WiFi.h>
// #include <WiFiClient.h>
// #include <HTTPClient.h>

// const char* ssid = "Kinneret College";
// WiFiClient client;

// void WiFi_Setup() {
//   WiFi.begin(ssid);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi connected");
// }

// void sendData(float temp, int light, int moisture) {
//   HTTPClient http;
//   http.begin("http://10.9.1.83:3011/esp/sendData");
//   http.addHeader("Content-Type", "application/json");

//   String jsonPayload = "{\"sensorName\": \"all\", \"plantNumber\": 1, \"temp\": " + String(temp) + 
//                        ", \"light\": " + String(light) + ", \"moisture\": " + String(moisture) + "}";
//   int httpCode = http.POST(jsonPayload);

//   if (httpCode == HTTP_CODE_OK) {
//     Serial.println("Data sent successfully!");
//   } else {
//     Serial.println("Failed to send data!");
//   }
  
//   http.end();
// }

// int GetState() {
//   HTTPClient http;
//   http.begin("http://10.9.1.83:3011/esp/state"); // לוודא שזה ה-IP הנכון
//   int httpCode = http.GET();

//   if (httpCode == HTTP_CODE_OK) {
//     String Res = http.getString();
//     Serial.print("Current state: ");
//     Serial.println(Res);
//     return Res.toInt();
//   }

//   http.end();
//   return -1;
// }


// // #include <WiFi.h>
// // #include <WiFiClient.h>
// // #include <HTTPClient.h>

// // const char* ssid = "Kinneret College";

// // WiFiClient client;

// // void WiFi_Setup(){
// //   WiFi.begin(ssid);

// //   while (WiFi.status() != WL_CONNECTED) {
// //     delay(500);
// //     Serial.print(".");
// //   }
// //   Serial.println("");
// //   Serial.println("wifi connected");
// // }
// // //הפונקציה מקבלת פרמטרים 

// // void sendData(float temp,int light,int moisture ){

// //   HTTPClient http;
// //   //כל מה שיהיה אחרי הסימן שאלה זה פרמטרים 
// //   String url="temp="+String(temp);
// //   //כאשר שולחים מספר נתונים חייבים &
// //   url+= "&light="+String(light);
// //   url+="&moisture="+String(moisture);
// //   Serial.println(url);
// //   http.begin(client,"http://10.9.1.83:3011/esp?"+url);
// //   //כששולחים דרך URL זה GET 
// //   int httpCode=http.GET();

// //   if(httpCode==HTTP_CODE_OK){
// //     Serial.print("HTTP RESPONSE CODE");
// //     Serial.println(httpCode);
// //   }

  

// //   http.end();
// // }
// // int GetState(){
// //   int ret = -1;
// //   HTTPClient http;
// //   // http.begin(client, "http://10.0.0.14:5050/esp/state"); // home ip
// //   http.begin(client, "http://10.9.0.19:5050/esp/state"); // Kinneret College ip
// //   int httpCode = http.GET();
// //   Serial.print(httpCode);
// //   if (httpCode == HTTP_CODE_OK){
// //   Serial.print("HTTP response code");
// //   Serial.print(httpCode); 
// //   String Res = http.getString();
// //   Serial.print(Res);
// //   ret = Res.toInt();
// //   }
// //   http.end();
// //   return ret;
// // }

// // String getJsonData(String state) {
// //   String Json = "";  
// //   HTTPClient http;
// //   http.begin(client, "http://10.0.0.14:5050/esp/DataMode?state=" + state);
// //   int httpCode = http.GET();
// //   Serial.print(httpCode);

// //   if (httpCode == HTTP_CODE_OK) {
// //     Serial.println(" HTTP response code: " + String(httpCode));
// //     Json = http.getString(); // שומר את התגובה ב-Json
// //     Serial.println("Response: " + Json);
// //   } else {
// //     Serial.println("Failed to get JSON data!");
// //   }

// //   http.end();
// //   return Json; // עכשיו זה מחזיר את המידע התקין
// // }


// // // String getJsonData(String state){
// // //   String Json = "";
// // //   HTTPClient http;
// // //   http.begin(client, "http://10.0.0.14:5050/esp/DataMode?state=" + state);
// // //   int httpCode = http.GET();
// // //   Serial.print(httpCode);
// // //   if (httpCode == HTTP_CODE_OK){
// // //   Serial.print("HTTP response code");
// // //   Serial.print(httpCode); 
// // //   String Res = http.getString();
// // //   Serial.print(Res);
// // //   }
// // //   http.end();
// // //   return Json;
// // // }