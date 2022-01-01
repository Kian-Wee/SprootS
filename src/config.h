#pragma once
#include <Arduino.h> //for platformio



/**
LIGHT AND ENVIRONMENT SENSORS
Uncomment Sensors in use
**/
//#define VEML7700 //Light Sensor
#define BH1750 //Light Sensor
//#define BME680 //Environment Sensor
//#define BME280 //Environment Sensor
#define BMP280 //Environment Sensor



// LIGHT SUBSYSTEM ----------------------------------------------------
#define LED 2 //LED PIN

// WATER SUBSYSTEM ----------------------------------------------------
#define pumpdis 14 //Pin for watering pump

// Select Growth Medium, uncomment medium in use
// To get a more accurate reading, measure the analog value of the moisture sensor of soil when it is completely dry and wet and edit the values
// The sparseness of the soil greatly affects the readings. Ensure that the soil sensor/probe is fully inserted into the soil
//#define SOIL_GENERIC
//#define HYDROTON_GENERIC
//#define SOIL_OGREENLIVING

class medium {
  public:
    String name;
    double retention;
    int dry;
    int wet;
};


// #if defined(SOIL_GENERIC)
// //58g of soil, 88g after absorbing water and some small soil losses
// #define mediumretention 0.517 // gram/ml of water per gram of soil
// #define mediumdry 3345
// #define mediumwet 1975
// //63.5g of 4-8mm clay, 71.5g after absorbing water
// #elif defined(HYDROTON_GENERIC)
// #define mediumretention 0.126 // gram/ml of water per gram of clay pellet
// #define mediumdry 3340
// #define mediumwet 2260
// #elif defined(SOIL_OGREENLIVING)
// #define mediumretention 0.517 // gram/ml of water per gram of soil
// #define mediumdry 1850
// #define mediumwet 1260
// #else
// #error "INVALID MEDIUM"
// #endif

// Water level(Deep water), not yet implemented
#define waterlvl 13

// Soil Moisture from capacitive sensor for different mediums
#define moistsens 34

//// IOT FUNCTIONALITY----------------------------------------------------

// Wifi 
//#define AutoConEN //Enable Autoconnect Library for easy wifi connection through web portal, takes up more flash memory

// Camera (NOT YET IMPLEMENTED)
// #define CAMERA_MODEL_AI_THINKER

// PCB enable to use power saving mosfet on pcb and invert LED Pins
// (NOT YET IMPLEMENTED)
#define POWON 13

// Touch pin
// Allows for user to prevent microcontroller to go into deep sleep after tapping it on,
// during normal operations it will poll all services once and go back to sleep
#define TouchThreshold 40 /* Greater the value, more the sensitivity */

// Voltage Sensor
#define voltagesens 15

//// VARIABLES----------------------------------------------------

//Placeholder extern variables until I figure out a better way
//Has to always be defined to reference to the main.cpp file on compile
extern bool PCBEN;
extern const bool BatteryEN;
extern bool LEDEN;
extern bool WaterEN;
extern bool Powersaving;
extern const bool TouchEN;
extern bool justwoke;
extern bool TeleEN;
extern bool ServerEN;
extern bool HTTPEN;
extern bool WifiEN;
extern const char* ssid;
extern const char* password;
extern bool RegisterEN;
extern const int defaultsleepdurationm;
extern const int defaultsleepdurations;
extern int sleepduration;
extern int lightreading;
extern double tempreading;
extern double pressurereading;
extern double humidityreading;
extern bool TeleUpdate;
extern bool Touched;
extern bool justwoke;
extern int touchtime;
extern int touchduration;
extern bool watered;
extern bool tankempty;
extern int mediumcapacity;
extern double variancepercentage;
extern double targetpercentage;
extern double wetnesspercentage;
extern double targetpercentage;
extern const int lowlight;
extern const int pumptimeout;
extern const int LVLI;
extern const String type;
extern float batterylevel;
extern const double desiredlight;
extern int hour;
extern bool quieth;
extern int startsleep;
extern int endsleep;
extern String version;