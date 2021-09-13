/*********
  Local Web Server. code adapted from
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/

//DNS
#include <ESPmDNS.h>
#include <WiFiClient.h>

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void webserversetup() {
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("sproot")) {
      Serial.println("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  Serial.println("mDNS responder started");

  server.begin();
  Serial.println("TCP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
}

void webserverloop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Sproot Web Server</h1>");
            
            // // Display current state, and ON/OFF buttons for GPIO 26  
            // client.println("<p>GPIO 26 - State " + output26State + "</p>");
            // // If the output26State is off, it displays the ON button       
            // if (output26State=="off") {
            //   client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            // } else {
            //   client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            // } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            if (tankempty == false){
              client.println("<p> Water tank State -> Filled </p>");
            }else{
              client.println("<p> Water tank State -> Empty </p>");
            }
            
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">FILLED</button></a></p>");
              tankempty = false;
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">EMPTY</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();

            // Water level
            client.println("<p>Current growth system type: " + String(type) + "</p>");
            int a = moisturepercentage();
            client.println("<p>Current moisture level: " + String(a) + "</p>");
            client.println("<p>Current light level: " + String(LDRL) + "</p>");
            client.println("<p>Tank empty check = " + String(tankempty) + "</p>");
            
            client.println("<p>Light level: " + String(lightreading) + "</p>");

            
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

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


