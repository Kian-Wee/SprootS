#include "water.h"
#include "data.h"

// extern String selected_medium = "soil_generic";
// extern double selected_retention=0;
// extern double selected_dry=0;
// extern double selected_wet=0;

// medium mediumarr;
medium mediumarr[3]={};

// Variable to store the current profile, may be modified directly if needed though settings cant be saved
String selected_medium = "soil_generic";
RTC_DATA_ATTR double selected_retention=0;
RTC_DATA_ATTR double selected_dry=1850;
RTC_DATA_ATTR double selected_wet=1300;


void setupWater(){

  // create file with presets on first boot
  if(createfile("/water.txt") == 0){

    //Create 3 different soil presets
    medium soil_generic;
    soil_generic.retention=0.517;
    soil_generic.dry=3345;
    soil_generic.wet=1975;

    medium hydroton_generic;
    hydroton_generic.retention=0.126;
    hydroton_generic.dry=3340;
    hydroton_generic.wet=2260;

    medium soil_ogreenliving;
    soil_ogreenliving.retention=0.517;
    soil_ogreenliving.dry=1850;
    soil_ogreenliving.wet=1200;

    medium mediumarr[3]={soil_generic,hydroton_generic,soil_ogreenliving};

    // iterate through list of mediums, convert to string for storage
    String tempstr = String("Medium_Name") + String("\t") + String("Water_Retention") + String("\t") + String("Dry_Value") + String("\t") + String("Wet_Value");
    for(auto const& i : mediumarr){
      tempstr += "\n";
      String astr= i.name + "\t" + i.retention + "\t" + i.dry + "\t" + i.wet;
      tempstr += astr;
    }
    storedata("/water.txt",tempstr); //write data in string format to text file
    }

  storemoisturesettings();

}

// Load dry and wet settings of current medium
void loadmediumsettings(String mediumname){
  // for (int i : mediumarr){ // loop through array //int i=0; i< length(mediumarr); i++
  //   if mediumarr[i].name==mediumname{
  //     extern int moistureretention=mediumarr[i].retention;
  //     extern int selected_dry=mediumarr[i].dry;
  //     extern int selected_wet=mediumarr[i].wet;
  //   }
  // }

}

//Goes through the list of mediums in the text file and stores or overwrites it
//Returns true if overwritten and false if not overwritten
bool writemediumsettings(medium newmedium){
  String tempstr=retrievedata("/water.txt");//read text file

  //Convert Text String into medium class

  //
    // for(i=0; i< sizeof(numbers)/sizeof(int);i++){
    for (auto const& i : mediumarr){ // loop through array //int i=0; i< length(mediumarr); i++
      //if medium already exist, overwrite settings
      if (i.name==newmedium.name){
        selected_medium=i.name;
        // CHECK WHY THERES ERROR
        // i.retention=double(newmedium.retention);

        // selected_retention=i.retention;
        // selected_dry=i.dry;
        // selected_wet=i.wet;
        return 1;
      }
    }

    //Medium not found, append current medium to end of file

    return 0;

}

// Placeholder function to estimate the volume of the pot so that the watering feature and timeout can be more accurate
void initwatercapacity(){
  mediumcapacity=0;
}

// Estimate the water remaining in the water bottle based on how much water was dispensed, starting from when flow was first detected and with a fixed flow rate
void estimatewatercapacity(){

}

// Print statement for moisture, currently unused
double moisturepercentage(){
  int currentreading = analogRead(moistsens);
  double percent = ( (-(currentreading-selected_wet)+(selected_dry-selected_wet)) / (selected_dry-selected_wet) )* 100;
  Serial.println(percent);
  return percent;
}

//Irrigates plant when growth medium is dry
//TODO CLEANUP NESTED IF STATEMENTS
void refillwater(){
// Currently there are 3 methods of watering
// Standard: waters whenever falls below the set moisture/wetness percentage
// Percentage(WIP): waters around a certain percentage within a threshold

  watered=false;

  //Calculates percentage of current wetness based on measured medium constants
  wetnesspercentage= double(selected_dry-analogRead(moistsens))/double(selected_dry-selected_wet);
  Serial.print("Wetness percentage: ");
  Serial.println(wetnesspercentage);

  //Checks if tank is previously emptied
  if(tankempty == true){
    Serial.println("Attempted to dispense water but tank is empty");

  //Watering for additional pump connected for deep water culture
  //Water until it hits a certain threshold and stops
  //PROBABLY CAN DEPRECIATE
  }else if(type=="Deep Water"){
    Serial.println("Deep Water dispensing");
    while(analogRead(waterlvl)<LVLI){
      digitalWrite(pumpdis,100);
      delay(2000);
    }

  // Threshold watering for soil/dutch bucket
  // Water whenever moisture level falls below set percentage
  }else if (type=="Standard"){
    Serial.print("Standard dispensing-----> ");

    int targetsum = selected_wet + (selected_dry-selected_wet)*(1-targetpercentage);

    if(analogRead(moistsens) > targetsum){
      Serial.print("Dispensing water");
      Serial.println(analogRead(moistsens));
      watered=true;
      int startTime;
      startTime=millis();
      int initalmoisture=analogRead(moistsens);

      Serial.print("Currently dispensing water: ");
      Serial.println(analogRead(moistsens));

      while(analogRead(moistsens) > targetsum && tankempty==false){
        digitalWrite(pumpdis,HIGH);
        Serial.println(analogRead(moistsens));

          //Timer to check if there is still water in reservior
          if (millis() - startTime >= pumptimeout*1000 && analogRead(moistsens) <= (initalmoisture + 60) && tankempty==false) { //10 threshold originally
            Serial.println("Water flow not detected, tank empty");
            tankempty=true;
            if (TeleEN== true){ //Send a telegram notification if enabled
              dmsendtelegrammessage("Water Tank is empty");
              digitalWrite(pumpdis,LOW);
            }
          }
      }
      digitalWrite(pumpdis,LOW);
      Serial.print("No longer dispensing water: ");
      Serial.println(analogRead(moistsens));
    }else{
      digitalWrite(pumpdis,LOW);
      Serial.print("Not Dispensing water: ");
      Serial.println(analogRead(moistsens));
    }
    
  // Percentage Watering
  // Waters when plant falls below targetpercentage - variancepercentage to targetpercentage + variancepercentage
  }else if (type=="Percentage"){
    Serial.println("Percentage dispensing");
    if (wetnesspercentage < (targetpercentage - variancepercentage)){
      Serial.println("Dispensing water");
      watered=true;

      int startTime; //start timer to check for timeout
      startTime=millis();
      int initalmoisture=analogRead(moistsens);
      //APPEND FOR MID RUNOUT

      while (wetnesspercentage < (targetpercentage + variancepercentage)){
        digitalWrite(pumpdis,HIGH);
        wetnesspercentage= double(selected_dry-analogRead(moistsens))/double(selected_dry-selected_wet);
        Serial.println("Dispensing water");
        Serial.print("Wetness percentage: ");
        Serial.println(wetnesspercentage);

        //Timer to check if there is still water in reservior
        if (millis() - startTime >= pumptimeout*1000 && analogRead(moistsens) <= (initalmoisture + 50) && tankempty==false) {
          Serial.println("Water flow not detected, tank empty");
          tankempty=true;
          //change to print message
          if (TeleEN== true){ //Send a telegram notification if enabled
            sendtelegrammessage("Water Tank is empty");
          }
        }

      }
      digitalWrite(pumpdis,LOW);
      Serial.println("No longer dispensing water");
    }else{
      digitalWrite(pumpdis,LOW);
      Serial.println("Not Dispensing water");
  }

}
}

void updatewater(){
  //Calculates percentage of current wetness based on measured medium constants
  wetnesspercentage= double(selected_dry-analogRead(moistsens))/double(selected_dry-selected_wet);
}