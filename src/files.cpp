

// /*
//   FSWebServer - Example WebServer with FS backend for esp8266/esp32
//   Copyright (c) 2015 Hristo Gochkov. All rights reserved.
//   This file is part of the WebServer library for Arduino environment.

//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.
//   You should have received a copy of the GNU Lesser General Public
//   License along with this library; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

//   upload the contents of the data folder with MkSPIFFS Tool ("ESP32 Sketch Data Upload" in Tools menu in Arduino IDE)
//   or you can upload the contents of a folder if you CD in that folder and run the following command:
//   for file in `ls -A1`; do curl -F "file=@$PWD/$file" esp32fs.local/edit; done

//   access the sample web page at http://esp32fs.local
//   edit the page by going to http://esp32fs.local/edit
// */
// // #include <WiFi.h>
// // #include <WiFiClient.h>
// // #include <WebServer.h>
// // #include <ESPmDNS.h>

// #define FILESYSTEM SPIFFS
// // You only need to format the filesystem once
// #define FORMAT_FILESYSTEM false

// // #if FILESYSTEM == FFat
// // #include <FFat.h>
// // #endif
// // #if FILESYSTEM == SPIFFS
// // #include <SPIFFS.h>
// // #endif

// const char* host = "esp32fs";
// // WebServer server(80);
// //holds the current upload
// File fsUploadFile;

// //format bytes
// String formatBytes(size_t bytes) {
//   if (bytes < 1024) {
//     return String(bytes) + "B";
//   } else if (bytes < (1024 * 1024)) {
//     return String(bytes / 1024.0) + "KB";
//   } else if (bytes < (1024 * 1024 * 1024)) {
//     return String(bytes / 1024.0 / 1024.0) + "MB";
//   } else {
//     return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
//   }
// }

// String getContentType(String filename) {
//   if (server.hasArg("download")) {
//     return "application/octet-stream";
//   } else if (filename.endsWith(".htm")) {
//     return "text/html";
//   } else if (filename.endsWith(".html")) {
//     return "text/html";
//   } else if (filename.endsWith(".css")) {
//     return "text/css";
//   } else if (filename.endsWith(".js")) {
//     return "application/javascript";
//   } else if (filename.endsWith(".png")) {
//     return "image/png";
//   } else if (filename.endsWith(".gif")) {
//     return "image/gif";
//   } else if (filename.endsWith(".jpg")) {
//     return "image/jpeg";
//   } else if (filename.endsWith(".ico")) {
//     return "image/x-icon";
//   } else if (filename.endsWith(".xml")) {
//     return "text/xml";
//   } else if (filename.endsWith(".pdf")) {
//     return "application/x-pdf";
//   } else if (filename.endsWith(".zip")) {
//     return "application/x-zip";
//   } else if (filename.endsWith(".gz")) {
//     return "application/x-gzip";
//   }
//   return "text/plain";
// }

// bool exists(String path){
//   bool yes = false;
//   File file = FILESYSTEM.open(path, "r");
//   if(!file.isDirectory()){
//     yes = true;
//   }
//   file.close();
//   return yes;
// }

// bool handleFileRead(String pathToFileName) {
//   Serial.println("handleFileRead: " + path);
//   if (path.endsWith("/")) {
//     path += "index.htm";
//   }
//   String contentType = getContentType(path, server);
//   String pathWithGz = path + ".gz";
//   if (exists(pathWithGz) || exists(path)) {
//     if (exists(pathWithGz)) {
//       path += ".gz";
//     }
//     File file = FILESYSTEM.open(path, "r");
//     server.streamFile(file, contentType);
//     file.close();
//     return true;
//   }
//   return false;
// }

// void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool finalt) {
//   if (server.uri() != "/edit") {
//     return;
//   }
//   HTTPUpload& upload = server.upload();
//   if (upload.status == UPLOAD_FILE_START) {
//     String filename = upload.filename;
//     if (!filename.startsWith("/")) {
//       filename = "/" + filename;
//     }
//     Serial.print("handleFileUpload Name: "); Serial.println(filename);
//     fsUploadFile = FILESYSTEM.open(filename, "w");
//     filename = String();
//   } else if (upload.status == UPLOAD_FILE_WRITE) {
//     //Serial.print("handleFileUpload Data: "); Serial.println(upload.currentSize);
//     if (fsUploadFile) {
//       fsUploadFile.write(upload.buf, upload.currentSize);
//     }
//   } else if (upload.status == UPLOAD_FILE_END) {
//     if (fsUploadFile) {
//       fsUploadFile.close();
//     }
//     Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
//   }
// }

// void handleFileDelete(AsyncWebServerRequest *request) {
//   if (server.args() == 0) {
//     return server.send(500, "text/plain", "BAD ARGS");
//   }
//   String path = server.arg(0);
//   Serial.println("handleFileDelete: " + path);
//   if (path == "/") {
//     return server.send(500, "text/plain", "BAD PATH");
//   }
//   if (!exists(path)) {
//     return server.send(404, "text/plain", "FileNotFound");
//   }
//   FILESYSTEM.remove(path);
//   server.send(200, "text/plain", "");
//   path = String();
// }

// void handleFileCreate(AsyncWebServerRequest *request) {
//   if (server.args() == 0) {
//     return server.send(500, "text/plain", "BAD ARGS");
//   }
//   String path = server.arg(0);
//   Serial.println("handleFileCreate: " + path);
//   if (path == "/") {
//     return server.send(500, "text/plain", "BAD PATH");
//   }
//   if (exists(path)) {
//     return server.send(500, "text/plain", "FILE EXISTS");
//   }
//   File file = FILESYSTEM.open(path, "w");
//   if (file) {
//     file.close();
//   } else {
//     return server.send(500, "text/plain", "CREATE FAILED");
//   }
//   server.send(200, "text/plain", "");
//   path = String();
// }

// void setup() {
//   if (FORMAT_FILESYSTEM) FILESYSTEM.format();
//   FILESYSTEM.begin();
//   {
//       File root = FILESYSTEM.open("/");
//       File file = root.openNextFile();
//       while(file){
//           String fileName = file.name();
//           size_t fileSize = file.size();
//           Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
//           file = root.openNextFile();
//       }
//       Serial.printf("\n");
//   }

//   //SERVER INIT
//   //load editor
//   server.on("/edit", HTTP_GET, [](AsyncWebServerRequest *request) {
//     if (!handleFileRead("/edit.htm")) {
//       request->send(404, "text/plain", "FileNotFound");
//     }
//   });
//   //create file
//   server.on("/edit", HTTP_PUT, handleFileCreate);
//   //delete file
//   server.on("/edit", HTTP_DELETE, handleFileDelete);
//   //first callback is called after the request has ended with all parsed arguments
//   //second callback handles file uploads at that location
//   server.on("/edit", HTTP_POST, [](AsyncWebServerRequest *request) {
//     request->send(200, "text/plain", "");
//   }, handleFileUpload); //HANDLE FILE UPLOAD CUSING ERROR
//   server.onFileUpload(handleFileUpload);

//   //called when the url is not defined here
//   //use it to load content from FILESYSTEM
//   server.onNotFound([](AsyncWebServerRequest *request) {
//     if (!handleFileRead(request->url())) {
//       request->send(404, "text/plain", "FileNotFound");
//     }
//   });

//   //get heap status, analog input value and all GPIO statuses in one json call
//   server.on("/all", HTTP_GET, [](AsyncWebServerRequest *request) {
//     String json = "{";
//     json += "\"heap\":" + String(ESP.getFreeHeap());
//     json += ", \"analog\":" + String(analogRead(A0));
//     json += ", \"gpio\":" + String((uint32_t)(0));
//     json += "}";
//     AsyncWebServerResponse *response = request->beginResponse(200);
//     response->addHeader("Server","ESP Async Web Server");
//     request->send(200, "text/json", json);
//     json = String();
//   });
//   server.begin();
//   Serial.println("HTTP server started");

// }

// // // void loop(void) {
// // //   server.handleClient();
// // //   delay(2);//allow the cpu to switch to other tasks
// // // }