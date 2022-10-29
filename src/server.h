#pragma once

#include "config.h"

#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>

String webupdate(const String& var);

// void notifyClients();

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

void asyncwebserversetup();

void FSBrowsersetup();

void updatedata();

void asyncwebserversetup();

void webserverloop();

