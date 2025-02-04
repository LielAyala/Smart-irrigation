#include <DHT.h> //שימוש בספרייה של DHT של טמפרטורה 
#include <ArdoinoJson>
//פין טמפרטורה ---------------------------------------------
#define dhtPIN 16// מקום אליו המכשיר מחובר 
#define DHTTYPE DHT11 //סוג החיישן שאנחנו עובדים איתו 

//----------------------------הגדרת חיישן טמפרטורה --------
//פין וסוג  בדומה לקריאה למחלקה 
DHT dht(dhtPIN, DHTTYPE); //המידע שיוחזר הוא מספר ממשי 
//-----------------------------------------------------------

//הגדרת חיישן אור ----------------------//לא מגדירים בSETUP 
#define lightSensor 32

//הגדרת חיישן לחות ----------------------//לא מגדירים בSETUP
#define Soil_moisture_Sensor 35

//פינים של משאבה -------------------------------------------
#define B1a 19
#define B1b 18


void setup(){
  //הפעלת החיישן טמפרטורה
	dht.begin(); 
  Serial.begin(115200);
  WiFi_Setup();

  dcMotorSetup();
  delay(100);

}

void loop(){
  float t=dht.readTemperature();
  //  Serial.print("temp=");//  Serial.print(t);
  int light= analogRead(lightSensor);
  // Serial.print("   light =");// Serial.print(light);
  int Soil_moisture=analogRead(Soil_moisture_Sensor);
  // Serial.print("   Soil_moisture =");// Serial.print(Soil_moisture);
  sendData(t,light,Soil_moisture);// Serial.println("");
  setOn();
  delay(2000);
  

}
// מצב עבודה של הפינים
void dcMotorSetup() {
    pinMode(B1a, OUTPUT);
    pinMode(B1b, OUTPUT);
}
//פונקציית הפעלה 
void setOn() {
	  digitalWrite(B1a, LOW);
	  digitalWrite(B1b, HIGH);
}
//פונקציית כיבוי 
void Off() {
    digitalWrite(B1a, HIGH);
    digitalWrite(B1b, HIGH);
}
//חיישן טמפרטורה 
void temp(){
  delay(5000);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius
    float t = dht.readTemperature();
     
    Serial.print("Temperature = ");
    Serial.println(t);
    Serial.print("Humidity = ");
    Serial.println(h);
}

void State machine(){

 enum state{
   off,
   on
  }
 state current_state=off;
 bool isItKlik=false;

}
