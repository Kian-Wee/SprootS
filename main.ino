#include <Arduino.h> //for platformio

//TODO
//RTC MEMOORY, EEPROM, HIBERNATE MODE
//ULP LIGHT AND WATER SUBSYSTEMS
//TODO proper print function for 

//Current WIP setup: standard irrigation/ dutch bucket
//TBD: deep water culture, NFT, Ebb and Flow

//automatic medium detection
//further reduce wifi connect time https://www.youtube.com/watch?v=IYuYTfO6iOs
//multiple ssid, ssid signal strength
//time pump and time stop pumping

//LED Light Memory Boosting
//Figure out time that light sets and count backwards if sun is provided
//subtract from light debt 10klux for 8 hours

// INPUTS----------------------------------------------------
//for wire library Wire.pins(SDA,SCL);
//for two wire, change the wire.begin(SDA,SCL)

const bool ADCEN = false; //enable external adc
// ~6.75 scaling factor for ads1115
// For 16bit ads1115, dry: 22400, wet: 12000, rough scaling factor, 6.75
int moistsensreading = 12000;

// Flow Sensor
// Capacitive flow sensor

//TODO, rewrite drivers for sensors
// Definition for sensors, Uncomment when needed
//#define VEML7700 //Light Sensor
#define BH1750 //Light Sensor
//#define BME680 //Environment Sensor
//#define BME280 //Environment Sensor
#define BMP280 //Environment Sensor

// Sensor Variables
// -updated every loop
int lightreading=-1;
double tempreading=-1; //MAYBE CHANGE TO FLOAT?
double pressurereading=-1;
double humidityreading=-1;

// LIGHT SUBSYSTEM ----------------------------------------------------

// Light Sensor
#define LDR 15
const double desiredlight = 0.8; // ratio, 1 being maximum sunlight;  change to crop specific sunlight
const int LDRL = 720; //morning and evening, half light
const int LDRH = 3000; //afternoon, full brightness, max value for ldr is 4096
double brightnesspercentage=0;
const int lowlight = 1000; //Light is considered low when below 100lux

// LED
#define LED 2 //2 is original pin
bool LEDEN = false; //RTC_DATA_ATTR , TODO, FIND OUT WIERD BUG THAT  HTTP NOR TELEGRAM CAN CHANGE THIS VARIABLE BUT NOT ACTIVATE LED
const int LEDmode=3; // 0-stable threshhold, 1-moving median, 2- weather prediction, 3-threshold
int LEDdelay=10; //number of seconds to recheck if light conditions have changed. Only applicable if sleep modes are disabled

//Advanced LED
int BrightnessArray[3]={0,0,0}; // Takes 3 historical readings and ignores the outliers
bool LEDState=0; //boolean to check if LED is off(0) or on(1)
int LEDstartTime=0; //start time variable for led delay

//TODO, HACKY METHOD OF RESETING THE TANKEMPTY WHEN USER CHANGES SOIL MOISTURE PARAMETER
// WATER SUBSYSTEM ----------------------------------------------------
// Water pump
bool WaterEN = false; //enabled if automatic irrigation system is avliable //RTC_DATA_ATTR bool WaterEN = false; //enabled if automatic irrigation system is avliable
//SEEMS TO BE AFFECTED BY IF SERIAL IS CONNECTED OR NOT
// TODO, CHANGE TO DEFINITION
const String type = "Standard"; // Watering mode
#define pumpdis 14
//#define pumpsuck 2
const double timetoml = 0; //convert duration the pump is on to ml dispensed
int mediumcapacity = 0;
bool watered = false; //notification variable if the plant has been watered

// Select Growth Medium
//#define SOIL_MEDIUM
//#define HYDROTON_MEDIUM
#define SOIL_NEW

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
#elif defined(SOIL_NEW)
const double mediumretention = 0.517; // gram/ml of water per gram of soil
const int mediumdry = 1850;
const int mediumwet = 1260;
#else
#error "iNVALID MEDIUM"
#endif

// Water level(Deep water)
#define waterlvl 13
const int LVLH = 0; //to log theoratical min and max values
const int LVLL = 0;
const int LVLI = 0; //ideal

//TODO COMPENSATE FOR ADC FOR DIFFERENT DEVICES AND DIFFERENT VOLTAGES
// Soil Moisture from capacitive sensor for different mediums
#define moistsens 34
const int dry = 3400; //measurement in only air
const int wet = 1700; //measurement in only water
const int pumptimeout = 10; //time taken for water to reach the pot, afterwhich it stops trying to pump and sends alert
bool tankempty = false;

// PLANT SPECIFIC
const int potcapacity = 1000; //grams of pot medium, needs to be caculated multiplied by retention rate
// Percentage
double wetnesspercentage = 0; //current wetness percentage, updated every call
const double variacepercentage = 0.2;
RTC_DATA_ATTR double targetpercentage = 0.5;


//// ELECTRONIC/IOT SUBSYSTEM----------------------------------------------------
// Wifi 
bool WifiEN = true;
const char* ssid = "";
const char* password = "";
const int wifirestartinterval=5;
bool AutoConEN = true; //Enable Autoconnect Library for easy wifi connection

// Time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 8*60*60;
const int   daylightOffset_sec = 0;
int hour = -1;

// Telegram
RTC_DATA_ATTR bool TeleEN = false; 
bool TeleUpdate = false; //Sends a status update message everytime the planter is online

// Web server
bool ServerEN = false;

// Json Server
bool JsonEN = false;

// HTTP connection for App
bool HTTPEN = true;
bool RegisterEN = true; // Web request for app registration

// Camera
bool CameraEN = false;
// #define CAMERA_MODEL_AI_THINKER

// AWS IOT
bool AWSEN = false;

// PCB enable to use power saving mosfet on pcb
bool PCBEN=true;
const int POWON=13;

////// POWER SAVING MODES-------------------------------------------------
//Future: different level of sleep modes for energy consumption
//0 - FULL power: esp32 is always on
//1 - Partial: wifi turned on perodically, ULP, bluetooth and wifi disabled when not in used
//2 - Perodic Deep Sleep: wifi only turned on when triggered or perodically, ULP
//3 - Adaptive deep sleep: Turns on often during the hot afternoon and less often at night
//4 - Predictive Adaptive deep sleep: Uses weather forecase to turn on during sunny hours
//5 - Automatic: Toggles sleep duration and power saving modes depending on how much power is remaining
// Power saving mode, goes into deep sleep after every cycle for sleepduration seconds
RTC_DATA_ATTR bool Powersaving = false; 
int powermode = 1;
const int defaultsleepdurationm = 30; // in minutes
const int defaultsleepdurations = 0; // in seconds
RTC_DATA_ATTR int sleepduration = defaultsleepdurations + defaultsleepdurationm*60;

// Touch pin
// Allows for user to prevent microcontroller to go into deep sleep after tapping it on,
// during normal operations it will poll all services once and go back to sleep
#define TouchThreshold 40 /* Greater the value, more the sensitivity */
const bool TouchEN =false;
int touchduration=60; //Time in seconds that the ESP32 will remain awake after using touch interrupt pin
int touchtime=0;
bool Touched=false;
bool justwoke=false;

// Voltage Sensor
const int voltagesens=15;
const double voltagescaling=1;
RTC_DATA_ATTR float batterylevel = 0; 
const bool BatteryEN = true;

//setup function to run through everytime the microcontroller starts or is awoken from deepsleep
void setup() {
  
  Serial.begin(115200);
  Serial.println("Initialising Serial");
  
  pinMode(pumpdis,OUTPUT);
  digitalWrite(pumpdis,LOW);

  if (PCBEN == true){
    Serial.println("Turning on Sensors");
    pinMode(POWON,OUTPUT);
    digitalWrite(POWON,HIGH);
  }

  if (BatteryEN==true){
    caculatebatterylevel(voltagesens);
  }

  //Initalises light and environment sensors if defined
  #if defined(VEML7700)
    setupVEML7700();
  #endif
  #if defined(BME680)
    setupBME680();
  #endif
  #if defined(BH1750)
    setupBH1750();
  #endif
  #if defined(BME280)
    setupBME280();
  #endif
  #if defined(BMP280)
    setupBMP280();
  #endif


  if (LEDEN == true){
    pinMode(LED,OUTPUT);
    if(PCBEN==false){
      digitalWrite(LED,HIGH);
    }else{
      digitalWrite(LED,LOW);//NMOS LOGIC
    }
  }

  if (WaterEN == true){
    pinMode(moistsens,INPUT);
  }

  if (Powersaving==true && TouchEN==true){
    touchAttachInterrupt(T0, callback, TouchThreshold);
    esp_sleep_enable_touchpad_wakeup();
    justwoke=true;
  }

  //sometimes monkey brain decides to turn on http without enabling wifi which results in wierd errors
  if ((TeleEN==true || ServerEN==true || AWSEN==true || HTTPEN==true || ServerEN==true || JsonEN==true) && WifiEN==false){
    Serial.println("Overwritting wifi param");
    WifiEN=true;
  }
  
  if (WifiEN==true){
    Serial.println("Setting up Wifi");
    if(AutoConEN==true){
      setupwifiauto();
    }else{
      setupwifi(ssid,password);
    }
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
  }

  if (TeleEN==true){
    Serial.println("Initalising Telegram");
    telesetup();
  }

  if (ADCEN == true){
    Serial.println("Initalising ADC");
    ads1115setup();
  }

  if (ServerEN == true){
    Serial.println("Initalising Webserver");
    webserversetup();
  }
  
  if (JsonEN == true){
    Serial.println("Initalising JSON Webserver");
    //jsonsetup();
  }

  if (HTTPEN == true){
    Serial.println("Initalising HTTP server");
  }

  if (AWSEN == true){
    Serial.println("Initalising AWS");
    connectAWS();
  }

  if (RegisterEN==true){
    Serial.println("Initalising Registration Web server");
    setupapp();
  }


}

void callback(){
  //placeholder callback function for touch wakeup
}

// Placeholder function to estimate the volume of the pot so that the watering feature and timeout can be more accurate
void initwatercapacity(){
  mediumcapacity=0;
}

// Estimate the water remaining in the water bottle based on how much water was dispensed, starting from when flow was first detected and with a fixed flow rate
void estimatewatercapacity(){

}

double moisturepercentage(){
  int currentreading = analogRead(moistsens);
  double percent = ( (-(currentreading-mediumwet)+(mediumdry-mediumwet)) / (mediumdry-mediumwet) )* 100;
  Serial.println(percent);
  return percent;
}

//Irrigates plant when growth medium is dry
//TODO CLEANUP NESTED IF STATEMENTS
void refillwater(){
// Currently there are 3 methods of watering
// Standard: waters whenever falls below the set moisture/wetness percentage
// Percentage(WIP): waters around a certain percentage within a threshold

  watered=false;

  //Calculates percentage of current wetness based on measured medium constants
  wetnesspercentage= double(mediumdry-analogRead(moistsens))/double(mediumdry-mediumwet);
  Serial.print("Wetness percentage: ");
  Serial.println(wetnesspercentage);

  //Checks if tank is previously emptied
  if(tankempty == true){
    Serial.println("Attempted to dispense water but tank is empty");

  //Watering for additional pump connected for deep water culture
  //Water until it hits a certain threshold and stops
  //PROBABLY CAN DEPRECIATE
  }else if(type=="Deep Water"){
    Serial.println("Deep Water dispensing");
    while(analogRead(waterlvl)<LVLI){
      digitalWrite(pumpdis,100);
      delay(2000);
    }

  // Threshold watering for soil/dutch bucket
  // Water whenever moisture level falls below set percentage
  }else if (type=="Standard"){
    Serial.print("Standard dispensing-----> ");

    int targetsum = mediumwet + (mediumdry-mediumwet)*(1-targetpercentage);

    if(analogRead(moistsens) > targetsum){
      Serial.print("Dispensing water");
      Serial.println(analogRead(moistsens));
      watered=true;
      int startTime;
      startTime=millis();
      int initalmoisture=analogRead(moistsens);

      Serial.print("Currently dispensing water: ");
      Serial.println(analogRead(moistsens));

      while(analogRead(moistsens) > targetsum && tankempty==false){
        digitalWrite(pumpdis,HIGH);

          //Timer to check if there is still water in reservior
          if (millis() - startTime >= pumptimeout*1000 && analogRead(moistsens) <= (initalmoisture + 60) && tankempty==false) { //10 threshold originally
            Serial.println("Water flow not detected, tank empty");
            tankempty=true;
            if (TeleEN== true){ //Send a telegram notification if enabled
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
    
  // Percentage Watering
  // Waters when plant falls below targetpercentage - variacepercentage to targetpercentage + variacepercentage
  }else if (type=="Percentage"){
    Serial.println("Percentage dispensing");
    if (wetnesspercentage < (targetpercentage - variacepercentage)){
      Serial.println("Dispensing water");
      watered=true;

      int startTime; //start timer to check for timeout
      startTime=millis();
      int initalmoisture=analogRead(moistsens);

      while (wetnesspercentage < (targetpercentage + variacepercentage)){ // 
        digitalWrite(pumpdis,HIGH);
        wetnesspercentage= double(mediumdry-analogRead(moistsens))/double(mediumdry-mediumwet);
        Serial.println("Dispensing water");
        Serial.print("Wetness percentage: ");
        Serial.println(wetnesspercentage);

        //Timer to check if there is still water in reservior
        if (millis() - startTime >= pumptimeout*1000 && analogRead(moistsens) <= (initalmoisture + 50) && tankempty==false) {
          Serial.println("Water flow not detected, tank empty");
          tankempty=true;
          if (TeleEN== true){ //Send a telegram notification if enabled
            sendtelegrammessage("Water Tank is empty");
          }
        }

      }
      digitalWrite(pumpdis,LOW);
      Serial.println("No longer dispensing water");
    }else{
      digitalWrite(pumpdis,LOW);
      Serial.println("Not Dispensing water");
  }

  

}
}

//Checks for ambient light and turns on/off led
void led(){
  Serial.print("Running LED Loop, ");
  //LEDState stands for current state of LED 1(ON), 0(OFF)
  //Serial.print("LED STATE: "); Serial.println(LEDState);

  //Thresholding on off, turn on if above threshold, else turn off
  if (LEDmode == 0){
    Serial.println("LED Mode 0");
    //LED is off, turn on if during allocated time hours
    if (millis() - LEDstartTime >= LEDdelay*1000) {
      LEDstartTime=millis();

      if(lightreading < lowlight && LEDState==0 && ((hour < 20 && hour > 8)||hour==-1) ){ // hour == -1 usecase for when wifi is not on
        Serial.println("Turning on LED");
        //N channel MOSFET for PCB(default off), RELAY for prototype(default ON)
        if(PCBEN==false){
          digitalWrite(LED,LOW);
        }else{
          digitalWrite(LED,HIGH);
        }
        LEDState=1;
        
      //LED is on, turn off led first and check ambient light sensor
      //If ambient light is high, dont check until next LEDdelay inverval
      //If ambient light is low, continue turning on LED
      }else if (LEDState==1){
        Serial.println("Testing ambient light sensor");
        if(PCBEN == false){
          digitalWrite(LED,HIGH);
          delay(100); //time for light sensor to react
        }else{
          digitalWrite(LED,LOW);
          delay(100); //time for the mosfet to discharge and light sensor to react
        }
        LEDState=0;
        
        #if defined(VEML7700)
          loopVEML7700();
        #endif
        #if defined(BH1750)
          loopBH1750();
        #endif

        if(lightreading < lowlight && LEDState==0 && ((hour < 20 && hour > 8)||hour==-1) ){
          Serial.println("Turning back on LED");
          if(PCBEN==false){
            digitalWrite(LED,LOW);
          }else{
            digitalWrite(LED,HIGH);
          }
          LEDState=1;
        }else{
          Serial.println("Turning off LED due to change in conditions");
          if(PCBEN==false){
            digitalWrite(LED,HIGH);
          }else{
            digitalWrite(LED,LOW);
          }
          
        }
      }
    }

  }else if (LEDmode == 1){
    // BrightnesArray[0]=BrightnesArray[1]; // Shift all readings down one position forgetting oldest reading
    // BrightnesArray[1]=BrightnessArray[2];
    // BrightnessArray[2] = lightreading; //Overwrite past reading with new reading
    // int SortedArray[3];
    // copy(begin(BrightnesArray), end(BrightnesArray), begin(SortedArray));
    // sort(SortedArray, SortedArray + 3);
    // Serial.print("Sorted Array: "); Serial.println(SortedArray);

  }else if(LEDmode ==3){
    Serial.println("LED Mode 3");
    if(lightreading < lowlight){
      Serial.println("Turning on LED");
      if(PCBEN==true){
        digitalWrite(LED,HIGH);
        Serial.println("LED SUCCESS");
      }else{
        digitalWrite(LED,LOW);
      }
    }else{
      Serial.println("Turning off LED");
      if(PCBEN==true){
        digitalWrite(LED,LOW);
      }else{
        digitalWrite(LED,HIGH);
      }
    }
  }

}

//simple test function
void checkanalogvalue(int analogpin){
  Serial.println(analogRead(analogpin));
  delay(500);
}

//simple test function for battery level
void caculatebatterylevel(int analogpin){
  // float R1 = 30000.0;
  // float R2 = 7520.0;
  float R1 = 470000.0;
  float R2 = 470000.0;
  float voltage_value = (analogRead(analogpin) * 3.3 ) / (4095);
  voltage_value = voltage_value / (R2/(R1+R2));
  Serial.print("Voltage level is: "); Serial.print(voltage_value);
  if(voltage_value==0){
    batterylevel=0;
  }else{
    batterylevel=(voltage_value-3.3)/(4.2-3.3)*100; //convert battery level to percentage
  }
  Serial.print(". Battery percentage is: "); Serial.println(batterylevel);
}

//placeholder function for light recommendation feature to check if there is enough light or not
void checksunlight(int analogpin, double desiredlight){
  if (analogRead(analogpin) < desiredlight && (hour > 18 or hour < 11)){
    Serial.println("Insufficient Light");
  }else{
    Serial.println("Sufficient Light");
  }
  brightnesspercentage=analogRead(LDR)/LDRH;
}

// main body, continously runs if deepsleep is disabled, else it runs through once
void loop(){

  if (Powersaving == true && TouchEN==true && justwoke==true){
    print_wakeup_reason();
  }

  if(PCBEN==true){
    digitalWrite(POWON,HIGH); //turn on sensors
  }

  //checkanalogvalue(moistsens);

  sleepduration = defaultsleepdurations + defaultsleepdurationm*60; //resets the sleep duration, can be overwritten by functions below

  if(WifiEN == true){
    Serial.print("Running Main Loop, Current Time: ");
    printLocalTime();
    if(AutoConEN==true){
      //Serial.println("Polling web portal");
      wifiautoloop();
    }
  }

  #if defined(VEML7700)
    loopVEML7700();
  #endif
  #if defined(BME680)
    loopBME680();
  #endif
  #if defined(BH1750)
    loopBH1750();
  #endif
  #if defined(BME280)
    loopBME280();
  #endif
  #if defined(BMP280)
    loopBMP280();
  #endif

  //External ADC
  if (ADCEN==true){
    Serial.print("Reading ADC: ");
    ads1115read();
  }

  //Check Brightness and on LED
  //checksunlight(LDR,LDRH*desiredlight);
  if(LEDEN==true){
    if(lightreading==-1){
      Serial.println("Not activating led system, light sensor not found");
    }else{
      pinMode(LED,OUTPUT);//TEMP BAND AID FIX FOR ACTIVATION OF LED IN APP
      led();
    }
  }

  //Refill water, only used if the app/http server is not enabled
  //if http server is enabled, it refills water between get and send request
  if (WaterEN==true && HTTPEN==false){
    refillwater();
  }
  
  //todo, resolve telegram and adc issue guru meditation error core panic
  //Telegram
  if (TeleEN == true){
    Serial.println("Running Telegram Loop");
    teleloop();

    String timestring = printLocalTime();
    String msgstring= timestring +"\n"+ String(lightreading) + "lux" + "\n" 
    + "Watered=" + watered+ "\n" 
    + "Battery Level=" + String(batterylevel)+ "\n"
    + "Soil Moisture=" + wetnesspercentage;
    Serial.print("Sending this"); Serial.println(msgstring);

    //dmsendtelegrammessagestring(msgstring);
    if (TeleUpdate == true && TouchEN==false){
      dmsendtelegrammessagestring(msgstring);
    }else if(TeleUpdate && TouchEN==true && justwoke==true){
      dmsendtelegrammessagestring(msgstring);
    }
  }

  //Web Server
  if (ServerEN == true){
    Serial.println("Running Web Server Loop");
    webserverloop();
  }
  
  if (AWSEN == true){
    Serial.println("Running AWS Loop");
    awsloop();
  }

  if(JsonEN == true){
    //jsonloop();
  }

  if(HTTPEN == true){
    HTTPloop();
  }

  //Sleep
  if (Powersaving == true){
    if(PCBEN==true){
      digitalWrite(POWON,LOW); //turn oFF sensors BEFORE SLEEP
    }
    //If Touch to wake is enabled, wait for set duration before going to sleep
    if(TouchEN==true&&Touched==true){
      Serial.println("Keeping ESP32 online due to touch interrupt");
      if(millis()>(touchtime+touchduration*1000)){
        Serial.println("Touch interupt ended, preparing to enter sleep");
        Touched=false; //resets touched variable back to false and goes back to sleep
        powermain(powermode);
      }
    }else{
      Serial.println("Preparing to enter sleep");
      powermain(powermode);
    }
    
  }

  Serial.println("-------------------------------------------------------");
  Serial.println();
  delay(1000);//delay loop if not in sleepmode to prevent spamming
}
