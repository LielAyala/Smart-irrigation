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