#pragma once

#include "config.h"
#include "power.h"
#include "sensor.h"

void handleNewMessagesunique(int numNewMessages);

void telesetup();

void teleloop();

// void sendtelegrammessage(const char* newmessage);

void sendtelegrammessage(String newmessage);

void dmsendtelegrammessage(String newmessage);