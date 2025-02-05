#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

const char* ssid = "Kinneret College";

WiFiClient client;

void WiFi_Setup(){
  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("wifi connected");
}
//הפונקציה מקבלת פרמטרים 

void sendData(float temp,int light,int moisture ){

  HTTPClient http;
  //כל מה שיהיה אחרי הסימן שאלה זה פרמטרים 
  String url="temp="+String(temp);
  //כאשר שולחים מספר נתונים חייבים &
  url+= "&light="+String(light);
  url+="&moisture="+String(moisture);
  Serial.println(url);
  http.begin(client,"http://10.9.1.83:3011/esp?"+url);
  //כששולחים דרך URL זה GET 
  int httpCode=http.GET();

  if(httpCode==HTTP_CODE_OK){
    Serial.print("HTTP RESPONSE CODE");
    Serial.println(httpCode);
  }

  

  http.end();
}
int GetState(){
  int ret = -1;
  HTTPClient http;
  // http.begin(client, "http://10.0.0.14:5050/esp/state"); // home ip
  http.begin(client, "http://10.9.0.19:5050/esp/state"); // Kinneret College ip
  int httpCode = http.GET();
  Serial.print(httpCode);
  if (httpCode == HTTP_CODE_OK){
  Serial.print("HTTP response code");
  Serial.print(httpCode); 
  String Res = http.getString();
  Serial.print(Res);
  ret = Res.toInt();
  }
  http.end();
  return ret;
}

String getJsonData(String state) {
  String Json = "";  
  HTTPClient http;
  http.begin(client, "http://10.0.0.14:5050/esp/DataMode?state=" + state);
  int httpCode = http.GET();
  Serial.print(httpCode);

  if (httpCode == HTTP_CODE_OK) {
    Serial.println(" HTTP response code: " + String(httpCode));
    Json = http.getString(); // שומר את התגובה ב-Json
    Serial.println("Response: " + Json);
  } else {
    Serial.println("Failed to get JSON data!");
  }

  http.end();
  return Json; // עכשיו זה מחזיר את המידע התקין
}


// String getJsonData(String state){
//   String Json = "";
//   HTTPClient http;
//   http.begin(client, "http://10.0.0.14:5050/esp/DataMode?state=" + state);
//   int httpCode = http.GET();
//   Serial.print(httpCode);
//   if (httpCode == HTTP_CODE_OK){
//   Serial.print("HTTP response code");
//   Serial.print(httpCode); 
//   String Res = http.getString();
//   Serial.print(Res);
//   }
//   http.end();
//   return Json;
// }