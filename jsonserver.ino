// //Local Server that publishes a json file to a local webpage
// //Uses ESP32 WebServer library + Autoconnect library
// #include <WebServer.h>
// #include <AutoConnect.h>
// #include <ESPmDNS.h>

// WebServer Server;
// AutoConnect Portal(Server);

// void rootPage() {

//   DynamicJsonDocument  doc(400);
//   doc["Light"] = lightreading; //Int
//   doc["Temperature"] = tempreading; //double
//   doc["Pressure"] = pressurereading; //double
//   doc["Humidity"] = humidityreading; //double
//   doc["Battery"] = batterylevel; //float

//   doc["Features"]["Water Subsystem"]=WaterEN; // boolean(T/F)
//   doc["Features"]["LED Subsystem"]= LEDEN; // boolean(T/F)

//   doc["Device Info"]["Firmware"]="Sproot V5"; //String
//   doc["Device Info"]["Microcontroller"] = "Firebeetle32"; //String
//   doc["Device Info"]["UID"] = 123456789; //Int, UserID

//   doc["Power Saving Mode"]["Enabled"]= Powersaving; // boolean(T/F), can be overwritten to disable sleep mode
//   doc["Power Saving Mode"]["Sleep Duration"]= sleepduration; //Int(Number of seconds microcontroller sleeps for), can be overwritten to modify sleep timing

//   doc["Alerts"]["Irrigation"]=watered; // boolean(T/F), Irrigation system has been turned on
//   doc["Alerts"]["LEDStatus"]=LEDState; // boolean(T/F), Current Status of LED Light(ON/OFF)

//   char jsonBuffer[1024];
//   serializeJson(doc, jsonBuffer); // print to client, serialises doc to jsonbuffer 

//   Server.send(200, "text/plain", jsonBuffer);
// }

// void jsonsetup() {
//   Server.on("/", rootPage);
//   if (Portal.begin()) {
//     Serial.println("WiFi connected: " + WiFi.localIP().toString());
//   }

//   if (!MDNS.begin("sproot")) {
//       Serial.println("Error setting up MDNS responder!");
//   }

//   Serial.println("mDNS responder started");
//   MDNS.addService("http", "tcp", 80);
// }

// void jsonloop() {
//     Serial.println("JSON server started on: " + WiFi.localIP().toString());
//     Portal.handleClient();
// }
