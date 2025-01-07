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

void sendData(){
  HTTPClient http;
  http.begin(client,"http://10.9.0.188:3011/esp/");
  int httpCode=http.GET();
  if(httpCode==HTTP_CODE_OK){
    Serial.print("HTTP RESPONSE CODE");
    Serial.println(httpCode);
  }
  http.end();
}