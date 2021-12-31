#pragma once

#include "config.h"

const int wifirestartinterval=5; //Time in s take for wifi to restart when it takes too long to connect
void setupwifi(const char* ssid,const char* password);


String printLocalTime();

#if defined(AutoConEN)
void setupwifiauto();
void wifiautoloop();
#endif

extern int hour; // declare as global variable