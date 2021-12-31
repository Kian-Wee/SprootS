#include "server.h"

/*********
  Local Web Server. code adapted from
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/

#include <ESPmDNS.h> //DNS
#include <SPIFFS.h>

//Initalises webpage variables on bootup/refresh
//Seems like if values are initalised here, it will not update with onmessage
//CANT BE LEFT EMPTY, HAS TO HAVE A VARIABLE THAT EXIST. ELSE WILL THROW HEAP ERROR
String webupdate(const String& var){
  // //Serial.println(var);
  if(var == "PLANT_INFO"){
    return "TEST";
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

void asyncwebserversetup(){

  Serial.println("Web Server Setup");

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("Sproot")) {
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
  doc["battery"] = 5; //float

  doc["Features"]["water_subsystem"]= WaterEN; // boolean(T/F)
  // targetpercentage=0.60; //use this to overwrite server
  doc["Features"]["target_moisture"]= int(targetpercentage*100); //double, target soil moisture percentage
  doc["Features"]["variance_moisture"]= int(variancepercentage*100);
  doc["Features"]["moisture"]= int(wetnesspercentage*100);
  doc["Features"]["LED_subsystem"]= LEDEN; // boolean(T/F)
  doc["Features"]["sufficient_light"]=0; // boolean(T/F)

  doc["Device_Info"]["Firmware"]="SprootS V1"; //String
  doc["Device_Info"]["Microcontroller"] = "Firebeetle32"; //String
  //doc["Device Info"]["UID"] = WiFi.macAddress(); //String, UserID in terms of MAC Address
  //doc["Device Info"]["UID"] = String(ESP.getChipId(), HEX);
  doc["Device_Info"]["UID"] = 1; //String, UserID in terms of MAC Address

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
