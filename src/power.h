#pragma once

#include "config.h"

//TODO: FIGURE OUT WHY Long duration(>40min) causes it to reset
// Main power function for either individual selection of power saving mode or automatic
bool powermain(int mode,int sleepduration);

float caculatebatterylevel(int analogpin);

void print_wakeup_reason();

void callback();