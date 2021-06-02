#include "awssecrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h> //https://arduinojson.org/v6/api/json/deserializejson/#example

extern int lightreading; // declare as global variable
extern double tempreading;
extern double pressurereading;
extern double humidityreading;
extern bool WaterEN;
extern bool LEDEN;
extern bool TeleEN;
extern bool ServerEN;
extern bool AWSEN;
extern bool Powersaving;
extern int sleepduration;
extern bool LEDState;
extern bool watered;

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

// WiFiClientSecure net = WiFiClientSecure();
WiFiClientSecure net; //initalised the same way
MQTTClient client = MQTTClient(256);

void connectAWS()
{
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.println("");
  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage()
{
  StaticJsonDocument<384> doc; //too small
  //DynamicJsonDocument doc(2048);
  doc["time"] = millis();
  doc["Light"] = lightreading;
  doc["Temperature"] = tempreading;
  doc["Pressure"] = pressurereading;
  doc["Humidity"] = humidityreading;

  doc["Features"]["Water Subsystem"]=WaterEN;
  doc["Features"]["LED Subsystem"]= LEDEN;
  doc["Features"]["Telegram Subsystem"]= TeleEN;
  doc["Features"]["Local Web Server"]= ServerEN;
  doc["Features"]["AWS"]= AWSEN;

  doc["Device Info"]["Firmware"]="Sproot V4";
  doc["Device Info"]["Microcontroller"] = "Firebeetle32";

  doc["Power Saving Mode"]["Enabled"]= Powersaving;
  doc["Power Saving Mode"]["Sleep Duration"]= sleepduration;

  doc["Alerts"]["Irrigation system has been turned on"]=watered;
  doc["Alerts"]["Status of LED lights"]=LEDState;

  //Serial.println("Publishing");
  char jsonBuffer[512];//512
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

//Recieve Topic from AWS
void messageHandler(String &topic, String &payload) {
 Serial.println("incoming: " + topic + " - " + payload);
 StaticJsonDocument<200> doc;
 deserializeJson(doc, payload);
 const char* message = doc["message"];
 const char* UID = doc["UID"];
 //Serial.println(message);
}

void awsloop() {
  publishMessage();
  client.loop();
  Serial.println("Publishing message to AWS");
}