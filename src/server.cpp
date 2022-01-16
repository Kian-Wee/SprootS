#include "server.h"

/*********
  Local Web Server. code adapted from
  Rui Santos at http://randomnerdtutorials.com  
*********/
// Other html reference taken from w3schools.com

#include <ESPmDNS.h> //DNS
#include <SPIFFS.h>

String devicename="Sproot"; //Name of webserver

//Initalises webpage variables on bootup/refresh
//Seems like if values are initalised here, it will not update with onmessage
//CANT BE LEFT EMPTY, HAS TO HAVE A VARIABLE THAT EXIST. ELSE WILL THROW HEAP ERROR
//remove init requires string
String webupdate(const String& var){
  if(var == "PLANT_INFO"){
    return "PLACEHOLDER";
  }
  //   // if(digitalRead(ledPin)){
  //   //   ledState = "ON";
  //   // }
  //   // else{
  //   //   ledState = "OFF";
  //   // }
  //   // Serial.print(ledState);
  //   // return ledState;
  // }else if(var=="TEMPERATURE"){
  //   return String(tempreading);
  // }else if(var=="LIGHT"){
  //   return String(lightreading);
  // }else if(var=="PRESSURE"){
  //   return String(pressurereading);
  // }else if(var=="HUMIDITY"){
  //   return String(humidityreading);
  // }else if(var=="TARGETPER"){
  //   return String(targetpercentage);
  // }else if(var=="VARIANCEPER"){
  //   return String(variancepercentage);
  // }
  
  //delete
  // if(var == "STATE"){
  //   if (ledState){
  //     return "ON";
  //   }
  //   else{
  //     return "OFF";
  //   }
  // }
  // return String();
}

String graphupdate(const String& var){

}



AsyncWebServer server(80); // Create AsyncWebServer object on port 80
AsyncWebSocket ws("/ws");

void notifyClients() {
  ws.textAll(String("ledState")); //temp
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    // if (strcmp((char*)data, "toggle") == 0) {
    //   ledState = !ledState;
    //   notifyClients();
    // }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// -------------------------------------------------------------------------------------------------------

// #include <ArduinoOTA.h>

#include <FS.h>
// #include <SPIFFS.h>
// #include <ESPmDNS.h>
// #include <WiFi.h>
#include <AsyncTCP.h>

#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>

// AsyncEventSource events("/events"); //Server Sent Events

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());

      if(info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());

      if((info->index + len) == info->len){
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}


// const char* ssid = "*******";
// const char* password = "*******";
const char * hostName = "esp-async";
const char* http_username = "admin";
const char* http_password = "admin";

void FSBrowsersetup(){
  // Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // WiFi.mode(WIFI_AP_STA);
  // WiFi.softAP(hostName);
  // WiFi.begin(ssid, password);
  // if (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //   Serial.printf("STA: Failed!\n");
  //   WiFi.disconnect(false);
  //   delay(1000);
  //   WiFi.begin(ssid, password);
  // }

  // //Send OTA events to the browser
  // ArduinoOTA.onStart([]() { events.send("Update Start", "ota"); });
  // ArduinoOTA.onEnd([]() { events.send("Update End", "ota"); });
  // ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
  //   char p[32];
  //   sprintf(p, "Progress: %u%%\n", (progress/(total/100)));
  //   events.send(p, "ota");
  // });
  // ArduinoOTA.onError([](ota_error_t error) {
  //   if(error == OTA_AUTH_ERROR) events.send("Auth Failed", "ota");
  //   else if(error == OTA_BEGIN_ERROR) events.send("Begin Failed", "ota");
  //   else if(error == OTA_CONNECT_ERROR) events.send("Connect Failed", "ota");
  //   else if(error == OTA_RECEIVE_ERROR) events.send("Recieve Failed", "ota");
  //   else if(error == OTA_END_ERROR) events.send("End Failed", "ota");
  // });
  // ArduinoOTA.setHostname(hostName);
  // ArduinoOTA.begin();

  MDNS.addService("http","tcp",80);
  mdns_hostname_set(devicename.c_str()); //MDNS name for easier access in local network

  // SPIFFS.begin();

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // events.onConnect([](AsyncEventSourceClient *client){
  //   client->send("hello!",NULL,millis(),1000);
  // });
  // server.addHandler(&events);

  server.addHandler(new SPIFFSEditor(SPIFFS, http_username,http_password)); //FS Browser library
  
  // server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(200, "text/plain", String(ESP.getFreeHeap()));
  // });

  server.serveStatic("/debug", SPIFFS, "index.htm"); //( url to be served at, SPIFFS, file to be served) 

  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.printf("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      Serial.printf("GET");
    else if(request->method() == HTTP_POST)
      Serial.printf("POST");
    else if(request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if(request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if(request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if(request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if(request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });
  server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char*)data);
    if(final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  });
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!index)
      Serial.printf("BodyStart: %u\n", total);
    Serial.printf("%s", (const char*)data);
    if(index + len == total)
      Serial.printf("BodyEnd: %u\n", total);
  });
  server.begin();
}

// void FSBrowserloop(){
//   ArduinoOTA.handle();
//   ws.cleanupClients();
// }

// -------------------------------------------------------------------------------------------------------

void asyncwebserversetup(){

  Serial.println("Web Server Setup");

  // Set up mDNS responder, allows webpage access on desktop(and safari) browsers through hostname
  if (!MDNS.begin(devicename.c_str())) {
      Serial.println("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  Serial.println("mDNS responder started");

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send(SPIFFS, "/web.html"); //this works but doesnt update values
    request->send(SPIFFS, "/web.html", String(), false, webupdate);  //send request on root(/) page to web.html
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  
  server.on("/graph", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/graph.html");
  });

  server.on("/setting", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Coming Soon");
  });
  // FSBrowser loaded by SPIFFSEditor

  // // Route to set GPIO to HIGH
  // server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
  //   digitalWrite(ledPin, HIGH);    
  //   request->send(SPIFFS, "/index.html", String(), false, processor);
  // });
  
  // // Route to set GPIO to LOW
  // server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
  //   digitalWrite(ledPin, LOW);    
  //   request->send(SPIFFS, "/index.html", String(), false, processor);
  // });

  initWebSocket();

  FSBrowsersetup();

  server.begin();
}

#include <ArduinoJson.h> // Include ArduinoJson Library

// Updates server by sending serialised json file
void updatedata(){
  DynamicJsonDocument doc(400);
  doc["light"] = lightreading; //Int
  doc["temperature"] = int(tempreading); //double
  doc["pressure"] = int(pressurereading); //double
  doc["humidity"] = int(humidityreading); //double
  doc["battery"] = batterylevel; //float

  doc["Features"]["water_subsystem"]= WaterEN; // boolean(T/F)
  doc["Features"]["target_moisture"]= int(targetpercentage*100); //double, target soil moisture percentage
  doc["Features"]["variance_moisture"]= int(variancepercentage*100);
  doc["Features"]["moisture"]= int(wetnesspercentage*100);
  doc["Features"]["LED_subsystem"]= LEDEN; // boolean(T/F)
  doc["Features"]["sufficient_light"]=0; // boolean(T/F)

  doc["Device_Info"]["Firmware"]=version; //String
  doc["Device_Info"]["Microcontroller"] = "Firebeetle32"; //String
  //doc["Device Info"]["UID"] = WiFi.macAddress(); //String, UserID in terms of MAC Address
  //doc["Device Info"]["UID"] = String(ESP.getChipId(), HEX);
  //doc["Device_Info"]["UID"] = 1; //String, UserID in terms of MAC Address

  doc["Power Saving Mode"]["Enabled"]= Powersaving; // boolean(T/F), can be overwritten to disable sleep mode
  doc["Power Saving Mode"]["Sleep Duration"]= sleepduration; //Int(Number of seconds microcontroller sleeps for), can be overwritten to modify sleep timing

  doc["Alerts"]["Irrigation"]=watered; // boolean(T/F), Irrigation system has been turned on
  //doc["Alerts"]["LEDStatus"]=LEDState; // boolean(T/F), Current Status of LED Light(ON/OFF)

  char jsonBuffer[1024];
  size_t len = serializeJson(doc, jsonBuffer); // print to client, serialises doc to jsonbuffer
  ws.textAll(jsonBuffer, len);
}

void webserverloop(){
  updatedata();
  ws.cleanupClients();
}

