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
