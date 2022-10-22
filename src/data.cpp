#include <FS.h>
#include <SPIFFS.h>
#include "data.h"
#include "Arduino.h"
#include <SPI.h>
#include <SD.h>
#include "esp.h"

// Store and retrieve data using SPIFFS to keep track of plant growth
// Reference https://diyprojects.io/esp32-get-started-spiff-library-read-write-modify-files/#.YWKgv9pByiP

// Currently using SPIFFS instead of LittleFS due to the more involved setup of LittleFS, might swap in the future if required

//String filename = "/data.txt";

// Store data, adds a new line of data everytime this is called
// returns true if successful
bool storedata(String filename, String data){

    // //CHECKED IN MAIN SKETCH ALREADY
    // //Check if SPIFFS can be opened
    // if(!SPIFFS.begin(true)){
    //     Serial.println("An Error has occurred while mounting SPIFFS");
    //     return false;
    // }

    //Check for avaliable memory
    if (SPIFFS.totalBytes()- SPIFFS.usedBytes() <10){
        Serial.println("Ran out of memory to store data");
        return false;
    }

    if(SPIFFS.exists(filename)==0){
        // File not found
        Serial.println("Failed to open file");
        return false;
    } else {
      File file = SPIFFS.open(filename, "a");
      // if (filename=="/water.txt"){ //water format
      //   file.println(data);
      //   file.close();
      //   return true;
      // }else{ //data storage format
      //   //Append Date and Time
      //   file.println(printLocalTime()); //from esp.cpp
      //   //Append Data
      //   file.println(consolidatedata()); // THERES A BUG, not saving
      //   file.close();
      //   Serial.println("Sucessfully modified file");
      //   return true;
      // }
      file.println(data);
      file.close();
      Serial.println("Sucessfully modified file");
      return true;
    }
}

String retrievedata(String filename){
  File file = SPIFFS.open(filename, "r");
  String tempstring = String(file.read());
  file.close();
  return tempstring;
}

bool createfile(String filename){

//   //Not sure if needed
//  bool formatted = SPIFFS.format();
//  if ( formatted ) {
//     Serial.println("SPIFFS formatted successfully");
//     return 0;
//   } else {
//     Serial.println("Error formatting");
//     return 0;
//   }
  if(SPIFFS.exists(filename)){
    Serial.println("File already exists");
    return 0;
  }else{
    Serial.println("Creating File");
    File file = SPIFFS.open(filename, "a");
    file.close();
    return 1;
  }
}

//Initalise file headers
String initplantfileheaders(){
  String tempstring="Date&Time";
  tempstring+="/t";
  if (WaterEN == true){
    tempstring += "Medium Moisture";
    tempstring += "/t";
  }
  if(tempreading != -274){
    tempstring += "Temperature";
    tempstring += "/t";
  }
  if(humidityreading !=-1){
    tempstring += "Humidity";
    tempstring += "/t";
  }
  if (pressurereading != -1){
    tempstring += "Pressure";
    tempstring += "/t";
  }
  if (BatteryEN ==true){
    tempstring += "Battery";
    tempstring += "/t";
  }
  return tempstring;
}

// Concatenates data into string for data logging
String consolidatedata(){
  String tempstring=printLocalTime();
  tempstring += "/t";
  if (WaterEN == true){
    tempstring += wetnesspercentage;
    tempstring += "/t";
  }
  if(tempreading != -274){
    tempstring += tempreading;
    tempstring += "/t";
  }
  if(humidityreading !=-1){
    tempstring += humidityreading;
    tempstring += "/t";
  }
  if (pressurereading != -1){
    tempstring += pressurereading;
    tempstring += "/t";
  }
  if (BatteryEN ==true){
    tempstring += batterylevel;
    tempstring += "/t";
  }
  return tempstring;
}

// Plant Specific Storing functions

// Store plant data in existing file
bool storeplantdata(String filename){
  return storedata(filename,consolidatedata());
}

// Setup new diary/text entry
bool setupplantdata(String filename){
  if(createfile(filename)==0) return 0;
  storedata(filename, initplantfileheaders());
  return 1;
}



// Setup new diary/text entry, checks if file exists and automatically increments filenames by 1 if current file is present
bool trysetupplantdata(String filename){
  for (int i = 0; i<10; i++){
    String newfilename = filename + "_" + String(i);
    if(setupplantdata(newfilename)==1){
      diaryname=newfilename; //Store the current filename as a variable and continue to store there in the future
      return 1;
    }
  }
  return 0; // File limit hit, more than 10 of the entries exist
}