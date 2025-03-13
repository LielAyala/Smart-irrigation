#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

// **פרטי WiFi**
const char* ssid = "ayala";
const char* password = "0502120218a";

WiFiClient client;
const char* serverUrl = "http://192.168.1.160:3011/esp/updateSensors";
const char* stateUrl = "http://192.168.1.160:3011/esp/state";

void WiFi_Setup() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP Address: ");
  Serial.println("WiFi Connected");
}

void SendData(float temp, int light, int moisture) {
  HTTPClient http;
  String dataURL = "Temp=" + String(temp);
  dataURL += "&Light=" + String(light);
  dataURL += "&Moisture=" + String(moisture);
  http.begin(client, "http://192.168.1.160:3011/esp?" + dataURL);  // home ip
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    Serial.print("HTTP response code");
    Serial.print(httpCode);
  }
  http.end();
}
//פונקציה לשליחת הדגימות
// פונקציה לשליחת דגימת חיישן לשרת
// void sendSample(String sensorName, float value) {
//     if (WiFi.status() == WL_CONNECTED) {
//         HTTPClient http;
//         http.begin("http://192.168.1.160:3011/esp/sendSample");
//         http.addHeader("Content-Type", "application/json");

//         // יצירת JSON תקין עם Escape לנתונים
//         String jsonPayload = "{\"sensorName\":\"" + sensorName + "\",\"measurementValue\":" + String(value, 2) + "}";

//         // הדפסת JSON לפני השליחה כדי לוודא תקינות
//         Serial.print("📌 JSON שנשלח: ");
//         Serial.println(jsonPayload);

//         // שליחת הבקשה לשרת
//         int httpResponseCode = http.POST(jsonPayload);

//         // בדיקת תגובת השרת
//         if (httpResponseCode > 0) {
//             Serial.println("✅ דגימה נשלחה בהצלחה! קוד תגובה: " + String(httpResponseCode));
//             Serial.println("📩 תגובת השרת: " + http.getString());
//         } else {
//             Serial.println("❌ שגיאה בשליחת הדגימה. קוד תגובה: " + String(httpResponseCode));
//         }

//         http.end();
//     } else {
//         Serial.println("⚠️ אין חיבור ל-WiFi! בדקו את הרשת.");
//     }
// }

void sendWaterUsage() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin("http://192.168.1.160:3011/esp/waterUsage");
        http.addHeader("Content-Type", "application/json");

        String jsonPayload = "{\"treeID\":" + String(treeID) + ",\"waterUsageToday\":" + String(waterUsageToday) + "}";

        int httpResponseCode = http.POST(jsonPayload);

        if (httpResponseCode > 0) {
            Serial.println(" נתוני מים נשלחו בהצלחה!");
        } else {
            Serial.println("שגיאה בשליחת הנתונים.");
        }

        http.end();
    }
}


//פונקציה שמקבלת את המצב הנוכחי מהinside_information
int getStatusFromServer() {
  HTTPClient http;  // יצירת אובייקט HTTPClient
  int ret = -1;     // משתנה להחזרת המצב מהשרת
  // כתובת ה-URL שמחזירה את המצב הנוכחי
  String url = "http://192.168.1.160:3011/esp/dataMode";
  Serial.print("🔹 שולח בקשת GET לכתובת: ");
  Serial.println(url);
  http.begin(url);            // פתיחת החיבור לשרת
  int httpCode = http.GET();  // שליחת בקשת GET
  // בדיקה אם השרת החזיר תשובה תקינה (200 OK)
  if (httpCode == HTTP_CODE_OK) {
    Serial.print("✅ קיבלתי תשובה מהשרת, קוד: ");
    Serial.println(httpCode);

    String response = http.getString();  // קריאת התגובה מהשרת
    Serial.print("📌 תוכן התשובה: ");
    Serial.println(response);
    // חיפוש הערך `currentState` בתגובה מהשרת
    int index = response.indexOf("currentState");
    if (index != -1) {
      response = response.substring(index + 14);                // חותך את הטקסט אחרי "currentState":
      response = response.substring(0, response.indexOf("}"));  // מסיר סוגר סוגר
      ret = response.toInt();                                   // ממיר למספר
    }
  } else {
    Serial.print("❌ שגיאה בחיבור לשרת, קוד: ");
    Serial.println(httpCode);
  }
  http.end();  // סגירת החיבור
  return ret;  // החזרת המצב שהתקבל מהשרת
}

int GetState() {
  int ret = -1;
  HTTPClient http;
  http.begin(client, "http://192.168.1.160:3011/esp/state");  // home ip
  int httpCode = http.GET();
  Serial.print(httpCode);
  if (httpCode == HTTP_CODE_OK) {
    Serial.print("HTTP response code");
    Serial.print(httpCode);
    String Res = http.getString();
    int start = Res.indexOf("\"CurrentStatus\":\"") + 17;  // מצא את המיקום של "CurrentStatus"
    int end = Res.indexOf("\"", start);                    // מצא את הסוף של הערך
    String stateStr = Res.substring(start, end);           // קח את הערך של הסטייט
    ret = stateStr.toInt();
    Serial.print("Res = ");
    Serial.println(Res);
    Serial.println("***********************");
    Serial.print("ret = ");
    Serial.println(ret);
    Serial.println("***********************");
  }
  http.end();
  return ret;
}

String getJsonData(String state) {
  String Json = "";
  HTTPClient http;

  // יצירת כתובת הבקשה לשרת עם הפרמטר המתאים
  String url = "http://192.168.1.160:3011/esp/dataMode?state=" + state;
  Serial.print("🔹 שולח בקשה לכתובת: ");
  Serial.println(url);

  http.begin(url);            // פתיחת החיבור
  int httpCode = http.GET();  // שליחת בקשת GET

  if (httpCode == HTTP_CODE_OK) {
    Serial.print("✅ קיבלתי תשובה מהשרת, קוד: ");
    Serial.println(httpCode);

    Json = http.getString();  // שמירת הנתונים שהתקבלו
    Serial.print("📌 תוכן התשובה: ");
    Serial.println(Json);
  } else {
    Serial.print("❌ שגיאה בחיבור לשרת, קוד: ");
    Serial.println(httpCode);
  }

  http.end();   // סגירת החיבור
  return Json;  // החזרת הנתונים שהתקבלו
}
