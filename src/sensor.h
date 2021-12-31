#pragma once

#include "config.h"

// extern int lightreading; // declare as global variable
// extern double tempreading;
// extern double pressurereading;
// extern double humidityreading;

#if defined(VEML7700)
void setupVEML7700();
void loopVEML7700();
#endif

#if defined(BH1750)
void setupBH1750();
void loopBH1750();
#endif

#if defined(BME680)
void setupBME680();
void loopBME680();
#endif

#if defined(BME280)
void setupBME280();
void loopBME280();
#endif

#if defined(BMP280)
void setupBMP280();
void loopBMP280();
#endif