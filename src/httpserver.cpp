#include "httpserver.h"

//char* serverName = ""; //Domain name with URL path or IP address with path
//char* publishName = ""; //Domain name with URL path or IP address with path

void setupapp(){
  String setupstring = "";
  const char* setupurl = setupstring.c_str();
  Serial.print("Sending setup to url: ");Serial.println(setupurl);

  HTTPClient http;    
  http.begin(setupurl);// Your Domain name with URL path or IP address with path

  http.addHeader("Content-Type", "application/json");
  DynamicJsonDocument doc(400);
  doc["1"] = 1; //Int to initalise account

  char jsonBuffer[1024];
  serializeJson(doc, jsonBuffer); // print to client, serialises doc to jsonbuffer
  int httpResponseCode = http.POST(jsonBuffer);
  Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
  Serial.println();

  http.end();
}

//1)pull http request
//2)publish http data to server
void HTTPloop() {

    Serial.println("Running HTTP Loop");

    setupapp();

    String getstring = "" + int64String(ESP.getEfuseMac(),DEC);
    const char* geturl = getstring.c_str();
    //const char* geturl = ""; //override
    Serial.print("Getting from url: ");Serial.println(geturl); 
    //Recieve(get) Data
    Serial.println("Geting data from server");
    String AppData = httpGETRequest(geturl);
    Serial.println(AppData);
    recievejson(AppData);
    Serial.println();

    if(WaterEN==true){
      refillwater();
      if(tankempty==true){
        targetpercentage=0;
      }
    }

    //Send(post) Data
    Serial.println("Publishing data to server");
    HTTPClient http;
    String poststring = "" + int64String(ESP.getEfuseMac(),DEC);
    const char* posturl = poststring.c_str();
    //const char* posturl = ""; //manual override
    Serial.print("Posting to url: ");Serial.println(posturl);      
    http.begin(posturl);// Your Domain name with URL path or IP address with path
    http.addHeader("Content-Type", "application/json");
    DynamicJsonDocument doc(400);
    doc["Light"] = lightreading; //Int
    // doc["Temperature"] = int(tempreading); //double
    // doc["Pressure"] = int(pressurereading); //double
    // doc["Humidity"] = int(humidityreading); //double
    doc["Temperature"] = 2; //double
    doc["Pressure"] = 3; //double
    doc["Humidity"] = 4; //double
    doc["Battery"] = 5; //float

    doc["Features"]["Water Subsystem"]= WaterEN; // boolean(T/F)
    // targetpercentage=0.60; //use this to overwrite server
    doc["Features"]["Moisture%"]= int(targetpercentage*100); //double, target soil moisture percentage
    doc["Features"]["LED Subsystem"]= LEDEN; // boolean(T/F)

    doc["Device Info"]["Firmware"]="Sproot V5"; //String
    doc["Device Info"]["Microcontroller"] = "Firebeetle32"; //String
    //doc["Device Info"]["UID"] = WiFi.macAddress(); //String, UserID in terms of MAC Address
    //doc["Device Info"]["UID"] = String(ESP.getChipId(), HEX);
    doc["Device Info"]["UID"] = 1; //String, UserID in terms of MAC Address

    doc["Power Saving Mode"]["Enabled"]= Powersaving; // boolean(T/F), can be overwritten to disable sleep mode
    doc["Power Saving Mode"]["Sleep Duration"]= sleepduration; //Int(Number of seconds microcontroller sleeps for), can be overwritten to modify sleep timing

    doc["Alerts"]["Irrigation"]=watered; // boolean(T/F), Irrigation system has been turned on
    //doc["Alerts"]["LEDStatus"]=LEDState; // boolean(T/F), Current Status of LED Light(ON/OFF)

    char jsonBuffer[1024];
    serializeJson(doc, jsonBuffer); // print to client, serialises doc to jsonbuffer
    Serial.println(jsonBuffer);
    int httpResponseCode = http.POST(jsonBuffer);

    Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
    Serial.println();
    
    //Free resources
    http.end();

}

// // serial print variable type , TEMPORARY FUNCTION
// void types(String a) { Serial.println("it's a String"); }
// void types(int a) { Serial.println("it's an int"); }
// void types(char *a) { Serial.println("it's a char*"); }
// void types(float a) { Serial.println("it's a float"); }
// void types(bool a) { Serial.println("it's a bool"); }

//GET: Request for data from http server
String httpGETRequest(const char* serverName) {
  HTTPClient http;
  http.begin(serverName);//IP address with path or Domain name with URL path 
  int httpResponseCode = http.GET(); // Send HTTP POST request
  
  String payload = "{}"; 
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();// Free resources
  return payload;
}

//TODO, TRY STATEMENT
//Recieve Json Data from Server
void recievejson(String payload){
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    Powersaving = doc["Powersaving"]; // boolean(T/F), can be overwritten to disable sleep mode
    sleepduration = doc["sleepduration"]; //Int(Number of seconds microcontroller sleeps for), can be overwritten to modify sleep timing
    WaterEN = doc["WaterEN"]; // boolean(T/F)
    LEDEN = doc["LEDEN"]; // boolean(T/F)
    int inttargetpercentage = doc["targetpercentage"]; //double
    targetpercentage = double(inttargetpercentage)/100;
    if(targetpercentage>0){
      tankempty=false;
    }
}