/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-esp32-cam-photo-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

//previously tested with 1.1.0, new compatability with 1.1.3 and json 6 is untested

#include <Arduino.h>

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h> //make sure library is 5.x



// Initialize Telegram BOT
String BOTtoken = "";  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
String CHAT_ID = "";

bool sendPhoto = false;

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);


//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

#define FLASH_LED_PIN 4
bool flashState = LOW;

String prevtext = "";

extern int lightreading; // declare as global variable
extern double tempreading;
extern double pressurereading;
extern double humidityreading;

void configInitCamera(){
  Serial.println("Running configInitCamera");

  // #if defined(CAMERA_MODEL_AI_THINKER)

  //CAMERA_MODEL_AI_THINKER
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27

  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22

// #endif

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  Serial.println("Allocating camera buffers");
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }
  
  Serial.println("Initalising camera");
  // camera init
  esp_err_t err = esp_camera_init(&config); //CAUSING CORE PANIC IF ADC IS INIT FIRST
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  Serial.println("Reducing frame size for frame rate");
  // Drop down frame size for higher initial frame rate
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_CIF);  // UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA
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
    if (text == "/start" or text == "/help" or text == "help" or text == "Help") {
      String welcome = "Welcome , " + from_name + "\n";
      welcome += "Use the following commands to interact with the ESP32-CAM \n";
      welcome += "/photo : takes a new photo\n";
      welcome += "/flash : toggles flash LED \n";
      welcome += "/pump : water plants \n";
      welcome += "/resettank : reset water tank after refilling \n";
      welcome += "/sensor : show sensor readings \n";
      welcome += "/moisture : checks moisture of soil \n";
      welcome += "/light : checks light intensity \n";
      bot.sendMessage(CHAT_ID, welcome, "");
    }
    if (text == "/flash") {
      flashState = !flashState;
      digitalWrite(FLASH_LED_PIN, flashState);
      Serial.println("Change flash LED state");
    }
    if (text == "/photo") {
      sendPhoto = true;
      Serial.println("New photo request");
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
    if (text== "/pump"){
      bot.sendMessage(CHAT_ID, "Target water percentage is: " + String(targetpercentage), "");
      bot.sendMessage(CHAT_ID, "Enter new percentage from(0 to 100):", "");
      //read message
    }
    if (prevtext== "/pump"){
      if (text.toInt() >=0 and text.toInt() <=100){
        targetpercentage=text.toInt()/100;
      }else{
        bot.sendMessage(CHAT_ID, "Invalid percentage", "");
      }
    }
    prevtext=text;

    if (text== "/resettank"){
      bot.sendMessage(CHAT_ID, "Water tank has now been filled", "");
      tankempty=false;
    }
  }
}

String sendPhotoTelegram() {
  const char* myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";

  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
    return "Camera capture failed";
  }  
  
  Serial.println("Connect to " + String(myDomain));


  if (clientTCP.connect(myDomain, 443)) {
    Serial.println("Connection successful");
    
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + CHAT_ID + "\r\n--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint16_t imageLen = fb->len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;
  
    clientTCP.println("POST /bot"+BOTtoken+"/sendPhoto HTTP/1.1");
    clientTCP.println("Host: " + String(myDomain));
    clientTCP.println("Content-Length: " + String(totalLen));
    clientTCP.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    clientTCP.println();
    clientTCP.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0;n<fbLen;n=n+1024) {
      if (n+1024<fbLen) {
        clientTCP.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        clientTCP.write(fbBuf, remainder);
      }
    }  
    
    clientTCP.print(tail);
    
    esp_camera_fb_return(fb);
    
    int waitTime = 10000;   // timeout 10 seconds
    long startTimer = millis();
    boolean state = false;
    
    while ((startTimer + waitTime) > millis())
    {
      Serial.print(".");
      delay(100);      
      while (clientTCP.available()) 
      {
          char c = clientTCP.read();
          if (c == '\n') 
          {
            if (getAll.length()==0) state=true; 
            getAll = "";
          } 
          else if (c != '\r')
            getAll += String(c);
          if (state==true) getBody += String(c);
          startTimer = millis();
       }
       if (getBody.length()>0) break;
    }
    clientTCP.stop();
    Serial.println(getBody);
  }
  else {
    getBody="Connected to api.telegram.org failed.";
    Serial.println("Connected to api.telegram.org failed.");
  }
  return getBody;
}

void telesetup(){
  Serial.println("Running telesetup");
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 

  // Set LED Flash as output
  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, flashState);

  // Config and init the camera
  if (CameraEN==true){
    Serial.println("Initalising Camera");
    configInitCamera();
  }

  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  Serial.println("Successfully initalised telesetup");
}

void teleloop() {

  if (sendPhoto) {
    if (CameraEN == true){
      Serial.println("Preparing photo");
      sendPhotoTelegram(); 
    }else{
      Serial.println("No camera detected, not sending photo");
    }
    sendPhoto = false; 
  }
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    
    Serial.println("checking for new message");
    
    //Serial.print("Message: ");
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    //Serial.println(numNewMessages);
    
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}

//Simple function to send telegram messages
void sendtelegrammessage(const char* newmessage){
  bot.sendMessage(CHAT_ID, String(newmessage),"");
  Serial.print("Sent this message via telegram: ");
  Serial.println(newmessage);
}

//Simple function to send telegram messages
void sendtelegrammessagestring(String newmessage){
  bot.sendMessage(CHAT_ID, newmessage,"");
  Serial.print("Sent this message via telegram: ");
  Serial.println(newmessage);
}
