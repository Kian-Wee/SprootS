#include "secrets.h" //Delete in release
#define INIT_CONFIG //init variables in config.h once
#include "config.h"

#include "esp.h"
#include "light.h"
#include "power.h"
#include "sensor.h"
#include "server.h"
#include "telegram.h"
#include "water.h"
#include "httpserver.h"

/** CHANGELOG
 * Restructured code functions and adapted from ino to cpp and header files
 * Changed from Sync webserver to async websocket server to remove sync issues and futureproof
 * Updated webpage to include realtime updates
 * Changed to define to save memory and speed up compile times for unsed features
 * Fixed Misc Typos and bugs
 * Misc Power consumption improvements, Fixed large integer sleep issue
 */

//Current WIP setup: standard irrigation/ dutch bucket
//TBD: deep water culture, NFT, Ebb and Flow

// //UNCOMMENT AND CHANGE THESE ITEMS
// const char* ssid = ""; //Wifi SSID
// const char* password = ""; //Wifi Password
// //Telegram Settings(Ignore if not using)
// String BOTtoken = "";  // Bot Token (Get from @Botfather)
// String CHAT_ID = ""; // Use @IDBot to find out the chat ID of an individual or a group, notifications are sent to this user(if configured)

String version="SprootS V1";

bool TelePrivate= 1; //If set to true(1), only the autohrised user(defined by CHAT_ID) is allowed to communicate with the bot, if set to false, anyone may communicate with it

// LIGHT SUBSYSTEM ----------------------------------------------------
const double desiredlight = 0.8; // ratio, 1 being maximum sunlight;  change to crop specific sunlight
double brightnesspercentage=0;
const int lowlight = 1000; //Light is considered low when below 100lux

// QUIET HOURS---------------------------------------------------------
// To minimise light and noise disruption from the led and the pump, set the sleep and wakeup time in which they will not be activated
bool quieth=true;
int startsleep=20; //8pm in 24h format
int endsleep=8; //8am in 24h format

// LED ----------------------------------------------------------------
RTC_DATA_ATTR bool LEDEN = false; //Enable or Disable external LED System
const int LEDmodeconfig=3; // 0-stable threshhold, 1-moving median, 2- weather prediction, 3-threshold

// WATER SUBSYSTEM ----------------------------------------------------
// Growth Mediums are defined in config.h
RTC_DATA_ATTR bool WaterEN = true; //enabled if automatic irrigation system is avliable
//SEEMS TO BE AFFECTED BY IF SERIAL IS CONNECTED OR NOT
const String type = "Standard"; // Watering mode
const double timetoml = 0; //convert duration the pump is on to ml dispensed
int mediumcapacity = 0;
bool watered = false; //notification variable if the plant has been watered



// Water level(Deep water), not yet implemented
const int LVLH = 0; //to log theoratical min and max values
const int LVLL = 0;
const int LVLI = 0; //ideal

// Soil Moisture from capacitive sensor for different mediums
const int dry = 3400; //measurement in only air (variable for comparison, not currently in use)
const int wet = 1700; //measurement in only water (variable for comparison, not currently in use)
const int pumptimeout = 10; //time taken for water to reach the pot, afterwhich it stops trying to pump and sends alert
RTC_DATA_ATTR bool tankempty = false; //true if water tank is detected to be empty

// PLANT SPECIFIC
const int potcapacity = 1000; //grams of pot medium, needs to be caculated multiplied by retention rate
double wetnesspercentage = 0; //current wetness percentage, updated every call
RTC_DATA_ATTR double variancepercentage = 0.2;
RTC_DATA_ATTR double targetpercentage = 0.5;

//// IOT FUNCTIONALITY----------------------------------------------------

// Wifi 
bool WifiEN = true;

// Time
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 8*60*60; //change to gmt timezone
const int daylightOffset_sec = 0;
int hour = -1; //current hour of the day

// Telegram
RTC_DATA_ATTR bool TeleEN = true; 
bool TeleUpdate = false; //Sends a status update message everytime the planter is online

// Web server
bool ServerEN = true;

// HTTP connection for App (deprecated)
bool HTTPEN = false;
bool RegisterEN = false; // Web request for app registration

// Camera
bool CameraEN = false;

// PCB enable to use power saving mosfet on pcb and invert LED Pins
bool PCBEN=true;

////// POWER SAVING MODES-------------------------------------------------
//Future: different level of sleep modes for energy consumption
//0 - FULL power: esp32 is always on
//1 - Partial: wifi turned on perodically, ULP, bluetooth and wifi disabled when not in used
//2 - Perodic Deep Sleep: wifi only turned on when triggered or perodically, ULP
//3 - Adaptive deep sleep: Turns on often during the hot afternoon and less often at night
//4 - Predictive Adaptive deep sleep: Uses weather forecase to turn on during sunny hours
//5 - Automatic: Toggles sleep duration and power saving modes depending on how much power is remaining
// Power saving mode, goes into deep sleep after every cycle for sleepduration seconds
RTC_DATA_ATTR bool Powersaving = true; 
int powermode = 1;
const int defaultsleepdurationm = 60; // in minutes
const int defaultsleepdurations = 0; // in seconds
RTC_DATA_ATTR int sleepduration = defaultsleepdurations + defaultsleepdurationm*60;

// Touch pin
// Allows for user to prevent microcontroller to go into deep sleep after tapping it on,
// during normal operations it will poll all services once and go back to sleep
const bool TouchEN =true;
int touchduration=60; //Time in seconds that the ESP32 will remain awake after using touch interrupt pin
int touchtime=0;
bool Touched=false;
bool justwoke=false;

// Voltage Sensor
const double voltagescaling=1;
RTC_DATA_ATTR float batterylevel = 0; 
const bool BatteryEN = true;

//-----------------------------------------------------------------------------------
// Sensor Variables
// -updated every loop
int lightreading=-1;
double tempreading=-274; //<0K
double pressurereading=-1;
double humidityreading=-1;



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
    setupWater();
  }

  if (Powersaving==true && TouchEN==true){
    touchAttachInterrupt(T0, callback, TouchThreshold);
    esp_sleep_enable_touchpad_wakeup();
    justwoke=true;
  }

  //turning on http without enabling wifi results in wierd errors
  if ((TeleEN==true || ServerEN==true  || HTTPEN==true || ServerEN==true) && WifiEN==false){
    Serial.println("Overwritting wifi param");
    WifiEN=true;
  }
  
  if (WifiEN==true){
    Serial.println("Setting up Wifi");

    #if defined(AutoConEN)
      setupwifiauto();
    #else
      setupwifi(ssid,password);
    #endif

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
  }

  if (TeleEN==true){
    Serial.println("Initalising Telegram");
    telesetup();
  }

  if (ServerEN == true){
    Serial.println("Initalising Webserver");
    //webserversetup();
    asyncwebserversetup();
  }

  if (HTTPEN == true){
    Serial.println("Initalising HTTP server");
  }

  if (RegisterEN==true){
    Serial.println("Initalising Registration Web server");
    setupapp();
  }

}



// main body, continously runs if deepsleep is disabled, else it runs through once before returning to sleep
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
    #if defined(AutoConEN)
      //Serial.println("Polling web portal");
      wifiautoloop();
    #endif
  }

  // //Temporarily disabled until gpio is changed from adc2
  // if (BatteryEN==true){
  //   batterylevel=caculatebatterylevel(voltagesens);
  // }

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

  //Check Brightness and on LED
  //checksunlight(LDR,LDRH*desiredlight);
  if(LEDEN==true){
    if(lightreading==-1){
      Serial.println("Not activating led system, light sensor not found");
    }else{
      led(LEDmodeconfig);
    }
  }

  //TEMP DEBUGGING
  //Refill water, only used if the app/http server is not enabled
  //if http server is enabled, it refills water between get and send request
  if (WaterEN==true && HTTPEN==false){
    if (quieth==false){
      refillwater();
    }else if (hour < startsleep && hour > endsleep){ //if quiet hours is activated, only refill during the day
      refillwater();
    }
    
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
      dmsendtelegrammessage(msgstring);
    }else if(TeleUpdate && TouchEN==true && justwoke==true){
      dmsendtelegrammessage(msgstring);
    }
  }

  //Web Server
  if (ServerEN == true){
    Serial.println("Running Web Server Loop");
    webserverloop();
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
      if(millis()>(touchtime+touchduration*1000)){
        Serial.println("Touch interupt ended, preparing to enter sleep");
        Touched=false; //resets touched variable back to false and goes back to sleep
        powermain(powermode,sleepduration);
      }
      Serial.print("Keeping ESP32 online due to touch interrupt, sleeping in ");
      Serial.print(touchtime/1000+touchduration-millis()/1000);
      Serial.println("s");
    }else{
      Serial.println("Preparing to enter sleep");
      powermain(powermode,sleepduration);
    }
    
  }

  Serial.println("-------------------------------------------------------");
  Serial.println();
  delay(1000);//delay loop if not in sleepmode to prevent spamming
}

//simple test function
void checkanalogvalue(int analogpin){
  Serial.println(analogRead(analogpin));
}

//print function to reduce serial overhead at runtime
//IMPORTANCE Levels
//1-information if the functions are running correctly, eg if wifi is intalised and it is watering
//2-sensor information
//3-logging
void print(String message, int importance, bool debug){
  if(debug==0){
    if(importance==1){
      Serial.println(message);
    }
  }else{
    if(importance==1){
      Serial.println(message);
      dmsendtelegrammessage(message);
    }else if(importance==2){
      Serial.println(message);
    }
  }
}

void println(String message, int importance, bool debug){
  if(debug==0){
    if(importance==1){
      Serial.println(message);
    }
  }else{
    if(importance==1){
      Serial.println(message);
      dmsendtelegrammessage(message);
    }else if(importance==2){
      Serial.println(message);
    }
  }
}