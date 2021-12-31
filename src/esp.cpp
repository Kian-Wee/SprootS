#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp.h"

void setupwifi(const char* ssid,const char* password){  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  
  int startTime;
  startTime=millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    // Restart wifi if takes too long to connect
    if (millis() - startTime >= wifirestartinterval*1000) {
      Serial.println("Restarting ESP due to wifi timeout");
      ESP.restart();
    }
  }
  Serial.println();
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP()); 

}

// Print time from NTP server with time.h library
#include "time.h"

String printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "Failed to obtain time";
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  // Serial.print("Time Hour: "); Serial.println(timeHour);
  hour=int(timeHour[0] - '0')*10 + int(timeHour[1] - '0'); // convert char to decimal
  char time[29];
  strftime(time,29, "%A, %B %d %Y %H:%M:%S", &timeinfo);
  // Serial.print("Time output: "); Serial.println(time);
  return time;
}

//Web Portal for wifi connection over web interface, disabled when not in use due to large size
#if defined(AutoConEN)

#include <AutoConnect.h>
AutoConnect Portal;
AutoConnectConfig Config;
#include <ESPmDNS.h>
#include <FS.h>
AutoConnectAux aux1("/register_device", "Register Device");

void setupwifiauto(){

  //Auxiliary register device page
  SPIFFS.begin();
  File page = SPIFFS.open("/register.json", "r");
  Portal.load(page);
  page.close();
  SPIFFS.end();
  Portal.join({aux1});
  Config.retainPortal = true; //Enable web portal to stay on even after wifi connection
  Config.apid = "sproot";
  Config.psk = "Iamsproot";
  Config.autoReconnect = true;    // Attempt automatic reconnection.
  Config.reconnectInterval = 6;   // Seek interval time is 180[s].
  Portal.config(Config);
  Portal.begin();
  if (MDNS.begin("sproot")) {
      MDNS.addService("http", "tcp", 80);
  }
}

void wifiautoloop(){
  Portal.handleClient();
}

#endif