// /*********
//   Rui Santos
//   Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-input-data-html-form/
  
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files.
  
//   The above copyright notice and this permission notice shall be included in all
//   copies or substantial portions of the Software.
// *********/

// #include <Arduino.h>
// #ifdef ESP32
//   #include <WiFi.h>
//   #include <AsyncTCP.h>
// #else
//   #include <ESP8266WiFi.h>
//   #include <ESPAsyncTCP.h>
// #endif
// #include <ESPAsyncWebServer.h>

// AsyncWebServer server(80);

// const char* PARAM_INPUT_1 = "input1";

// //DNS
// #include <ESPmDNS.h>

// // HTML web page to handle 3 input fields (input1, input2, input3)
// const char index_html[] PROGMEM = R"rawliteral(
// <!DOCTYPE HTML><html><head>
//   <title>ESP Input Form</title>
//   <meta name="viewport" content="width=device-width, initial-scale=1">
//   </head><body>
//   <form action="/get">
//     input1: <input type="text" name="input1">
//     <input type="submit" value="Submit">
//   </form>
// </body></html>)rawliteral";

// void notFound(AsyncWebServerRequest *request) {
//   request->send(404, "text/plain", "Not found");
// }

void setupregister() {

// // Set up mDNS responder:
//   // - first argument is the domain name, in this example
//   //   the fully-qualified domain name is "esp8266.local"
//   // - second argument is the IP address to advertise
//   //   we send our IP address on the WiFi network
//   if (!MDNS.begin("sproot")) {
//       Serial.println("Error setting up MDNS responder!");
//       while(1) {
//           delay(1000);
//       }
//   }
//   Serial.println("mDNS responder started");

//   server.begin();
//   Serial.println("TCP server started");

//   // Add service to MDNS-SD
//   MDNS.addService("http", "tcp", 80);



//   // Send web page with input fields to client
//   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
//     request->send_P(200, "text/html", index_html);
//   });

//   // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
//   server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
//     String inputMessage;
//     String inputParam;
//     // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
//     if (request->hasParam(PARAM_INPUT_1)) {
//       inputMessage = request->getParam(PARAM_INPUT_1)->value();
//       inputParam = PARAM_INPUT_1;
//     }
//     else {
//       inputMessage = "No message sent";
//       inputParam = "none";
//     }
//     Serial.println(inputMessage);
//     request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
//                                      + inputParam + ") with value: " + inputMessage +
//                                      "<br><a href=\"/\">Return to Home Page</a>");
//   });
//   server.onNotFound(notFound);
//   server.begin();
}