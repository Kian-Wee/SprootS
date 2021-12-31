/*
  Code adapted from Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-esp32-cam-photo-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
// #include "esp_camera.h"
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "telegram.h"

extern String BOTtoken;
extern String CHAT_ID;

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

String prevtext = "";

//for only a single person
void handleNewMessagesunique(int numNewMessages) {
  Serial.print("Handle New Messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    
    /*
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    */

    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);
    
    String from_name = bot.messages[i].from_name;
    if (text == "/start" or text == "/help" or text == "help" or text == "Help") {
      String welcome = "Welcome , " + from_name + "\n";
      welcome += "Use the following commands to interact with the Sproot \n";
      welcome += "/web : print ip address of web portal \n";
      welcome += "/pump : water plants \n";
      welcome += "/resettank : reset water tank after refilling \n";
      welcome += "/sensor : show sensor readings \n";
      welcome += "/moisture : checks moisture of soil \n";
      welcome += "/light : checks light intensity \n";
      welcome += "/admin : override light and pump system to run in admin mode \n";
      welcome += "/LEDON : override and turn on led \n";
      welcome += "/LEDOFF : override and turn off led \n";
      welcome += "/PUMPON : override and turn on pump \n";
      welcome += "/PUMPOFF : override and turn off pump \n";
      bot.sendMessage(CHAT_ID, welcome, "");
    }

    if (text== "/sensor"){
      String message="Soil Moisture:" + String(wetnesspercentage*100) + "\n";
      message +="Light:" + String(lightreading) +"lux" + "\n";
      message +="Temperature:" + String(tempreading) +"C" + "\n";
      message +="Pressure:" + String(pressurereading) + "\n";
      message +="Humidity:" + String(humidityreading) + "\n";
      bot.sendMessage(CHAT_ID, message, "");
    }
    if (text== "/moisture"){
      bot.sendMessage(CHAT_ID, String(wetnesspercentage*100), "");
    }
    if (text== "/light"){
      bot.sendMessage(CHAT_ID, String(lightreading), "");
    }
    if (text== "/temperature"){
      bot.sendMessage(CHAT_ID, String(tempreading), "");
    }
    if (text== "/pressure"){
      bot.sendMessage(CHAT_ID, String(pressurereading), "");
    }
    if (text== "/humidity"){
      bot.sendMessage(CHAT_ID, String(humidityreading), "");
    }
    if (text== "/moisture"){
      bot.sendMessage(CHAT_ID, "Target water percentage is: " + String(targetpercentage), "");
      bot.sendMessage(CHAT_ID, "Enter new percentage from(0 to 100):", "");
      //read message
    }
    if (prevtext== "/moisture"){
      if (text.toInt() >=0 and text.toInt() <=100){
        targetpercentage=text.toInt()/100;
      }else{
        bot.sendMessage(CHAT_ID, "Invalid percentage", "");
      }
    }
    prevtext=text;

    if (text== "/PUMPON"){
      digitalWrite(pumpdis,HIGH);
      bot.sendMessage(CHAT_ID, "Turned on pump", "");
    }
    if (text== "/PUMPOFF"){
      digitalWrite(pumpdis,LOW);
      bot.sendMessage(CHAT_ID, "Turned off pump", "");
    }
    if (text== "/LEDON"){
      digitalWrite(LED,HIGH);
      bot.sendMessage(CHAT_ID, "Turned on LED", "");
    }
    if (text== "/LEDOFF"){
      digitalWrite(LED,LOW);
      bot.sendMessage(CHAT_ID, "Turned off LED", "");
    }

    if (text== "/resettank"){
      bot.sendMessage(CHAT_ID, "Water tank has now been filled", "");
      tankempty=false;
    }

    if (text== "/battery"){
      caculatebatterylevel(voltagesens);
      bot.sendMessage(CHAT_ID, String(batterylevel), "");
    }

    if (text== "/admin"){
      //pinMode(pumpdis,INPUT);
      WaterEN=false;
      LEDEN=false;
      bot.sendMessage(CHAT_ID, "Admin mode activated, subsystems deactivated", "");
    }

    if (text== "/web" or text== "/webportal" or text== "/ip"){
      bot.sendMessage(CHAT_ID, String("http://"+ WiFi.localIP().toString()), "");
    }
  }
}

void handleNewMessages(int numNewMessages) {
  Serial.print("Handle New Messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    /*
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    */
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);
    
    String from_name = bot.messages[i].from_name;
    //if (text == "/start" or text == "/help" or text == "help" or text == "Help" or text == "HELP") {
    if (text.equalsIgnoreCase("/start") == true || text.equalsIgnoreCase("/help") == true) { 
      String welcome = "Welcome , " + from_name + "\n";
      welcome += "Use the following commands to interact with the ESP32-CAM \n";
      welcome += "/web : send web server ip address \n";
      welcome += "/pump : water plants \n";
      welcome += "/resettank : reset water tank after refilling \n";
      welcome += "/sensor : show sensor readings \n";
      welcome += "/moisture : checks moisture of soil \n";
      welcome += "/light : checks light intensity \n";
      welcome += "/LEDON : override and turn on led \n";
      welcome += "/LEDOFF : override and turn off led \n";
      welcome += "/PUMPON : override and turn on pump \n";
      welcome += "/PUMPOFF : override and turn off pump \n";
      bot.sendMessage(bot.messages[i].chat_id, welcome, "");
    }

    if (text== "/sensor"){
      String message="Soil Moisture:" + String(wetnesspercentage*100) + "\n";
      message +="Light:" + String(lightreading) +"lux" + "\n";
      message +="Temperature:" + String(tempreading) +"C" + "\n";
      message +="Pressure:" + String(pressurereading) + "\n";
      message +="Humidity:" + String(humidityreading) + "\n";
      bot.sendMessage(bot.messages[i].chat_id, message, "");
    }
    if (text== "/moisture"){
      bot.sendMessage(bot.messages[i].chat_id, String(wetnesspercentage*100), "");
    }
    if (text== "/light"){
      bot.sendMessage(bot.messages[i].chat_id, String(lightreading), "");
    }
    if (text== "/temperature"){
      bot.sendMessage(bot.messages[i].chat_id, String(tempreading), "");
    }
    if (text== "/pressure"){
      bot.sendMessage(bot.messages[i].chat_id, String(pressurereading), "");
    }
    if (text== "/humidity"){
      bot.sendMessage(bot.messages[i].chat_id, String(humidityreading), "");
    }
    if (text== "/moisture"){
      bot.sendMessage(bot.messages[i].chat_id, "Target water percentage is: " + String(targetpercentage), "");
      bot.sendMessage(bot.messages[i].chat_id, "Enter new percentage from(0 to 100):", "");
      //read message
    }
    if (prevtext== "/moisture"){
      if (text.toInt() >=0 and text.toInt() <=100){
        targetpercentage=text.toInt()/100;
      }else{
        bot.sendMessage(bot.messages[i].chat_id, "Invalid percentage", "");
      }
    }
    prevtext=text;

    if (text== "/PUMPON"){
      digitalWrite(pumpdis,HIGH);
      bot.sendMessage(bot.messages[i].chat_id, "Turned on pump", "");
    }
    if (text== "/PUMPOFF"){
      digitalWrite(pumpdis,LOW);
      bot.sendMessage(bot.messages[i].chat_id, "Turned off pump", "");
    }
    if (text== "/LEDON"){
      digitalWrite(LED,HIGH);
      bot.sendMessage(bot.messages[i].chat_id, "Turned on LED", "");
    }
    if (text== "/LEDOFF"){
      digitalWrite(LED,LOW);
      bot.sendMessage(bot.messages[i].chat_id, "Turned off LED", "");
    }

    if (text== "/LEDSTATEON"){
      LEDEN=true;
      bot.sendMessage(bot.messages[i].chat_id, "Turned on LED State", "");
    }

    if (text== "/resettank"){
      bot.sendMessage(bot.messages[i].chat_id, "Water tank has now been filled", "");
      tankempty=false;
    }

    if (text== "/battery"){
      caculatebatterylevel(voltagesens);
      bot.sendMessage(bot.messages[i].chat_id, String(batterylevel), "");
    }
    if (text== "/web" or "/web_portal" or "/portal"){
      bot.sendMessage(bot.messages[i].chat_id, "http://" + String(WiFi.localIP()), "");
    }
  }
}

void telesetup(){
  Serial.println("Running telesetup");

  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  bot.updateToken(BOTtoken); //fix for universaltelegrambot library not initalising with the bot token in an external file

  Serial.println("Successfully initalised telesetup");
}

void teleloop() {

  if (millis() > lastTimeBotRan + botRequestDelay)  {
    
    Serial.print("Checking for new telegram messages, ");
    
    Serial.print("Message: ");
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    Serial.println(numNewMessages);
    
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessagesunique(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}

// //Simple function to send telegram messages
// void sendtelegrammessage(const char* newmessage){
//   bot.sendMessage(bot.messages[0].chat_id, String(newmessage),"");
//   Serial.print("Sent this message via telegram: ");
//   Serial.println(newmessage);
// }

//Simple function to send telegram messages
void sendtelegrammessage(String newmessage){
  bot.sendMessage(bot.messages[0].chat_id, String(newmessage),"");
  Serial.print("Sent this message via telegram: ");
  Serial.println(newmessage);
}

//Simple function to send telegram messages to specific chat id
void dmsendtelegrammessage(String newmessage){
  bot.sendMessage(CHAT_ID, newmessage,"");
  Serial.print("Sent this message via telegram: ");
  Serial.println(newmessage);
}
