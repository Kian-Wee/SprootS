
// CODE IS DEPRECIATED, REFER TO SERVER.CPP INSTEAD

#pragma once

// extern int lightreading; // declare as global variable
// extern double tempreading;
// extern double pressurereading;
// extern double humidityreading;
// extern bool LEDEN;
// extern bool WaterEN;
// extern bool Powersaving;
// extern int sleepduration;
// extern double targetpercentage;
// extern bool tankempty;

// HTTP Webserver to post and recieve data for mobile application, adapted from: https://randomnerdtutorials.com/esp32-http-get-post-arduino/#http-get-2

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Int64String.h>
#include "config.h"
#include "water.h"

void setupapp();

void HTTPloop();

String httpGETRequest(const char* serverName);

void recievejson(String payload);