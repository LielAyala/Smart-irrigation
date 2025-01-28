#include <WiFi.h>
#include <HTTPClient.h>

// פרטי חיבור לרשת Wi-Fi
const char* ssid = "שם_הרשת_שלך";
const char* password = "הסיסמה_שלך";

// כתובת השרת
const char* serverUrl = "192.168.1.160";

void setup() {
  Serial.begin(115200); // הפעלת חיבור סריאלי
  delay(1000);

  // התחברות ל-Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("מתחבר ל-Wi-Fi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nחיבור לרשת הצליח!");
  Serial.print("כתובת IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http; // יצירת אובייקט HTTPClient
    
    http.begin(serverUrl); // הגדרת כתובת השרת
    http.addHeader("Content-Type", "application/json"); // הוספת כותרת
    
    // שליחת בקשת POST עם נתונים לדוגמה
    String jsonPayload = "{\"message\": \"Hello from ESP32!\"}";
    int httpResponseCode = http.POST(jsonPayload);
    
    if (httpResponseCode > 0) {
      Serial.print("קוד תגובה: ");
      Serial.println(httpResponseCode);
      String response = http.getString();
      Serial.println("תשובת השרת:");
      Serial.println(response);
    } else {
      Serial.print("שגיאה בבקשה: ");
      Serial.println(httpResponseCode);
    }
    
    http.end(); // סיום החיבור
  } else {
    Serial.println("איבדנו חיבור ל-Wi-Fi...");
  }
  
  delay(10000); // השהיה של 10 שניות בין כל בקשה
}
