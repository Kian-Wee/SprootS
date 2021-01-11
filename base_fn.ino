//Current WIP setup: standard irrigation/ dutch bucket
//TBD: deep water culture, NFT, Ebb and Flow

const String type = "Standard percentage soil"; 
// INPUTS----------------------------------------------------
// Water level(Deep water)
#define waterlvl 13
const int LVLH = 0; //to log theoratical min and max values
const int LVLL = 0;
const int LVLI = 0; //ideal

//TODO COMPENSATE FOR ADC FOR DIFFERENT DEVICES AND DIFFERENT VOLTAGES
// Soil Moisture from capacitive sensor for different mediums
#define moistsens 12
const int dry = 3400; //measurement in only air
const int wet = 1700; //measurement in only water
const int timeout = 60 * 1000;
//58g of soil, 88g after absorbing water and some small soil losses
const double soilretention = 0.517; // gram/ml of water per gram of soil
const int soildry = 3345;
const int soilwet = 1975;
//63.5g of 4-8mm clay, 71.5g after absorbing water
const double clayretention = 0.126; // gram/ml of water per gram of clay pellet
const int claydry = 3340;
const int claywet = 2260;
// PLANT SPECIFIC
const int potcapacity = 1000; //grams of pot medium, needs to be caculated multiplied by retention rate
// Percentage
double wetnesspercentage = 0; //current wetness percentage, updated every call
const double variacepercentage = 0.1;
const double targetpercentage = 0.6;

// Light Sensor
#define LDR 15
const double desiredlight = 0.8; // ratio, 1 being maximum sunlight;  change to crop specific sunlight
const int LDRL = 720; //morning and evening, half light
const int LDRH = 3000; //afternoon, full brightness, max value for ldr is 4096
double brightnesspercentage=0;


// Flow Sensor
// Capacitive flow sensor

// OUTPUTS----------------------------------------------------
// LED
#define LED 1
const bool LEDEN=false;

// Water pump
#define pumpdis 14
#define pumpsuc 2
const double timetoml = 0; //convert duration the pump is on to ml dispensed
int mediumcapacity = 0;

//// IOT----------------------------------------------------
// Wifi 
bool WifiEN = false;
const char* ssid = "";
const char* password = "";

// Time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 8 *60*60;
const int   daylightOffset_sec = 0;

// Telegram
const bool TeleEN = false;

// Web server
const bool ServerEN = false;

// Camera


// Power saving mode, goes into deep sleep after every cycle for sleepduration seconds
const bool Powersaving = false; 
const int sleepduration = 10; // in seconds


void setup() {
  
  Serial.begin(115200);
  Serial.println("Initialising Serial");
  
  pinMode(LED,OUTPUT);
  pinMode(pumpdis,OUTPUT);
  pinMode(pumpsuc,OUTPUT);
  pinMode(waterlvl,INPUT);

  

  if (TeleEN==true or ServerEN==true){
    WifiEN=true;
  }
  
  if (WifiEN==true){
    setupwifi(ssid,password);
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
//    char hour=printLocalTime();
//    Serial.println("Hour: ",hour);
  }

  if (TeleEN==true and WifiEN==true){
    Serial.println("Initalising Telegram");
    telesetup();//initalise telegram app setup
  }
  
}

void initwatercapacity(){
  mediumcapacity=0;
}

void refillwater(){
  
  if(type=="Deep Water"){
    while(analogRead(waterlvl)<LVLI){
      digitalWrite(pumpdis,100);
      delay(2000);
    }

  // Threshold watering for soil/dutch bucket
  }else if (type=="Standard"){
//    if(analogRead(moistsens) > claydry-100){
//      Serial.println("Dispensing water");
//      while(analogRead(moistsens) > claywet){
//        digitalWrite(pumpdis,HIGH);
//      }
    if(analogRead(moistsens) > 2000){
      Serial.print("Dispensing water");
      Serial.println(analogRead(moistsens));
      while(analogRead(moistsens) > 2000){
        digitalWrite(pumpdis,HIGH);
      }
      digitalWrite(pumpdis,LOW);
      Serial.println("No longer dispensing water");
    }else{
      digitalWrite(pumpdis,LOW);
      Serial.println("Not Dispensing water");
    }
    
  }else if (type=="Standard percentage soil"){
    wetnesspercentage= double(claydry-analogRead(moistsens))/double(claydry-claywet);
//    Serial.println(analogRead(moistsens));
    Serial.print("Wetness percentage: ");
    Serial.println(wetnesspercentage);
    if (wetnesspercentage < targetpercentage - variacepercentage){
      Serial.println("Dispensing water");
      while (wetnesspercentage < targetpercentage + variacepercentage){
        digitalWrite(pumpdis,HIGH);
        wetnesspercentage= double(claydry-analogRead(moistsens))/double(claydry-claywet);
        Serial.println("Dispensing water");
//        Serial.println(analogRead(moistsens));
        Serial.print("Wetness percentage: ");
        Serial.println(wetnesspercentage);
      }
      digitalWrite(pumpdis,LOW);
      Serial.println("No longer dispensing water");
    }else{
      digitalWrite(pumpdis,LOW);
      Serial.println("Not Dispensing water");
  }
  // TIMEOUT WHEN EMPTY WATER
  //TODO proper print function for 

}
}

int checktime(){
  //setupwifi();
  return 1800;
}

void led(){
  if(checktime()>1800 or analogRead(LDR)<LDRL){
    digitalWrite(LED,HIGH);
  }else{
    digitalWrite(LED,LOW);
  }
}

void checkanalogvalue(int analogpin){
  Serial.println(analogRead(analogpin));
  delay(500);
}

void checksunlight(int analogpin, double desiredlight){
  if (analogRead(analogpin) < desiredlight and (checktime() > 1800 or checktime() < 1100)){
    Serial.println("Insufficient Light");
    if(LEDEN==true){
      digitalWrite(LED,HIGH);}
  }else{
    Serial.println("Sufficient Light");
    if(LEDEN==true){
      digitalWrite(LED,LOW);}
  }
  brightnesspercentage=analogRead(LDR)/LDRH;
}

void loop(){
//  Serial.println("Running Main Loop");

  //Check Brightness and on LED
  //checksunlight(LDR,LDRH*desiredlight);

  //checkanalogvalue(2);
  //Refill water
  refillwater();
  delay(500);

  //Telegram
  if (TeleEN == true){
    teleloop();
  }

  //Sleep
  if (Powersaving == true){
    esp_sleep_enable_timer_wakeup(sleepduration * 1000000); //convert from ns to s
    esp_deep_sleep_start();
  }  
}
