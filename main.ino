#include <Arduino.h> //for platformio

//Current WIP setup: standard irrigation/ dutch bucket
//TBD: deep water culture, NFT, Ebb and Flow

//automatic medium detection

//Future: different level of sleep modes for energy consumption
//FULL: esp32 is always on
//Partial: wifi turned on perodically, ULP, bluetooth and wifi disabled when not in used
//Minimal: wifi only turned on when triggered or perodically, ULP

// Watering mode
// TODO, CHANGE TO DEFINITION
const String type = "Standard";

// Select Growth Medium
//#define SOIL_MEDIUM
#define HYDROTON_MEDIUM

// INPUTS----------------------------------------------------
//for wire library Wire.pins(SDA,SCL);
//for two wire, change the wire.begin(SDA,SCL)

const bool ADCEN = false; //enable external adc
// ~6.75 scaling factor for ads1115
// For 16bit ads1115, dry: 22400, wet: 12000, rough scaling factor, 6.75
int moistsensreading = 12000;

// Water level(Deep water)
#define waterlvl 13
const int LVLH = 0; //to log theoratical min and max values
const int LVLL = 0;
const int LVLI = 0; //ideal

//TODO COMPENSATE FOR ADC FOR DIFFERENT DEVICES AND DIFFERENT VOLTAGES
// Soil Moisture from capacitive sensor for different mediums
#define moistsens 36
const int dry = 3400; //measurement in only air
const int wet = 1700; //measurement in only water
const int pumptimeout = 30; //time taken for water to reach the pot, afterwhich it stops trying to pump and sends alert
bool tankempty = false;

#if defined(SOIL_MEDIUM)
//58g of soil, 88g after absorbing water and some small soil losses
const double mediumretention = 0.517; // gram/ml of water per gram of soil
const int mediumdry = 3345;
const int mediumwet = 1975;
//63.5g of 4-8mm clay, 71.5g after absorbing water
#elif defined(HYDROTON_MEDIUM)
const double mediumretention = 0.126; // gram/ml of water per gram of clay pellet
const int mediumdry = 3340;
const int mediumwet = 2260;
#else
#error "iNVALID MEDIUM"
#endif

// PLANT SPECIFIC
const int potcapacity = 1000; //grams of pot medium, needs to be caculated multiplied by retention rate
// Percentage
double wetnesspercentage = 0; //current wetness percentage, updated every call
const double variacepercentage = 0.1;
double targetpercentage = 0.6;



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
const int wifirestartinterval=5;

// Time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 8 *60*60;
const int   daylightOffset_sec = 0;

// Telegram
const bool TeleEN = false;

// Web server
const bool ServerEN = true;

// Camera
const bool CameraEN = false;
// #define CAMERA_MODEL_AI_THINKER

// Power saving mode, goes into deep sleep after every cycle for sleepduration seconds
const bool Powersaving = false; 
const int sleepduration = 60*10; // in seconds


void setup() {
  
  Serial.begin(115200);
  Serial.println("Initialising Serial");
  
  // pinMode(LED,OUTPUT);
  pinMode(pumpdis,OUTPUT);
  // pinMode(pumpsuc,OUTPUT);
  // pinMode(waterlvl,INPUT);



  if ( (TeleEN==true || ServerEN==true) && WifiEN==false){
    Serial.println("Overwritting wifi param");
    WifiEN=true;
  }
  
  if (WifiEN==true){
    Serial.println("Setting up Wifi");
    setupwifi(ssid,password);
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
//    char hour=printLocalTime();
//    Serial.println("Hour: ",hour);
  }

  if (TeleEN==true){
    Serial.println("Initalising Telegram");
    telesetup();//initalise telegram app setup
  }
  if (ADCEN == true){ //
    ads1115setup();
  }

  if (ServerEN == true){
    webserversetup();
  }
}



void initwatercapacity(){
  mediumcapacity=0;
}

double moisturepercentage(){
  int currentreading = analogRead(moistsens);
  double percent = -( ((currentreading-mediumwet)-(mediumdry-mediumwet)) / (mediumdry-mediumwet) * 100);
  Serial.println(((currentreading-mediumwet)-(mediumdry-mediumwet)));
  return percent;
}

// Currently there are 3 methods of watering
// Standard: waters whenever falls below the set moisture/wetness percentage
// Standard percentage(WIP): waters around a certain percentage within a threshold
// Timing: waters every x interval to the preset moisture level
void refillwater(){

  if( tankempty == true){
    Serial.println("Attempted to dispense water but tank is empty");

  }else if(type=="Deep Water"){
    Serial.println("Deep Water dispensing");
    while(analogRead(waterlvl)<LVLI){
      digitalWrite(pumpdis,100);
      delay(2000);
    }

  // Threshold watering for soil/dutch bucket
  }else if (type=="Standard"){
    Serial.print("Standard dispensing-----> ");
//    if(analogRead(moistsens) > claydry-100){
//      Serial.println("Dispensing water");
//      while(analogRead(moistsens) > claywet){
//        digitalWrite(pumpdis,HIGH);
//      }

    int targetsum = mediumwet + (mediumdry-mediumwet)*(1-targetpercentage);

    if(analogRead(moistsens) > targetsum){
      Serial.print("Dispensing water");
      Serial.println(analogRead(moistsens));
      int startTime;
      startTime=millis();
      int initalmoisture=analogRead(moistsens);
      while(analogRead(moistsens) > targetsum && tankempty==false){
        digitalWrite(pumpdis,HIGH);
        // ads1115read();
        Serial.print("Currently dispensing water: ");
        Serial.println(analogRead(moistsens));

          if (millis() - startTime >= pumptimeout*1000 && analogRead(moistsens) <= (initalmoisture + 10) && tankempty==false) {
            Serial.println("Water flow not detected, tank empty ");
            tankempty=true;
            if (TeleEN== true){
              sendtelegrammessage("Water Tank is empty");
            }
            
          }
      }
      digitalWrite(pumpdis,LOW);
      Serial.print("No longer dispensing water: ");
      Serial.println(analogRead(moistsens));
    }else{
      digitalWrite(pumpdis,LOW);
      Serial.print("Not Dispensing water: ");
      Serial.println(analogRead(moistsens));
    }
    
  }else if (type=="Standard percentage soil"){
    Serial.println("Standard percentage soil dispensing");
    wetnesspercentage= double(mediumdry-analogRead(moistsens))/double(mediumdry-mediumwet);
//    Serial.println(analogRead(moistsens));
    Serial.print("Wetness percentage: ");
    Serial.println(wetnesspercentage);
    if (wetnesspercentage < targetpercentage - variacepercentage){
      Serial.println("Dispensing water");
      while (wetnesspercentage < targetpercentage + variacepercentage){
        digitalWrite(pumpdis,HIGH);
        wetnesspercentage= double(mediumdry-analogRead(moistsens))/double(mediumdry-mediumwet);
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

//placeholder function, change to read from wifi
int checktime(){
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

  //Serial.println(moisturepercentage());

  // checkanalogvalue(moistsens);

  if(WifiEN == true){
    Serial.print("Running Main Loop, Current Time: ");
    printLocalTime();
  }

  //adc
  // if (ADCEN==true){
  //   Serial.print("Reading ADC: ");
  //   ads1115read();
  // }

  // //Check Brightness and on LED
  // //checksunlight(LDR,LDRH*desiredlight);

  // //Refill water
  //refillwater();

  //todo, resolve telegram and adc issue guru meditation error core panic
  //Telegram
  if (TeleEN == true){
    teleloop();
  }

  //Web Server
  if (ServerEN == true){
    webserverloop();
  }
  
  //Sleep
  if (Powersaving == true){
    Serial.print("Sleeping for: ");
    Serial.println(sleepduration);
    esp_sleep_enable_timer_wakeup(sleepduration * 1000000); //convert from ns to s
    esp_deep_sleep_start();
  }  
}
