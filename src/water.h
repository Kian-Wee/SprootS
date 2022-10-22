#pragma once

#include "config.h"
#include "telegram.h"

void setupWater();

void loadmediumsettings(String mediumname);

bool writemediumsettings(medium newmedium);

void initwatercapacity();

void estimatewatercapacity();

double moisturepercentage();

void refillwater(String type);

void updatewater();