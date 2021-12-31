#include "light.h"
#include "config.h"
#include "sensor.h"

//extern const int lowlight;

//Advanced LED config
int LEDdelay=1; //number of seconds to recheck if light conditions have changed. Only applicable if sleep modes are disabled
int BrightnessArray[3]={0,0,0}; // Takes 3 historical readings and ignores the outliers
bool LEDState=0; //boolean to check if LED is off(0) or on(1)
int LEDstartTime=0; //start time variable for led delay

//Checks for ambient light and turns on/off led
void led(int LEDmode){

  Serial.print("Running LED Loop, ");
  //LEDState stands for current state of LED 1(ON), 0(OFF)
  //Serial.print("LED STATE: "); Serial.println(LEDState);

  //Thresholding on off, turn on if above threshold, else turn off
  if (LEDmode == 0){
    Serial.println("LED Mode 0");
    //LED is off, turn on if during allocated time hours
    if (millis() - LEDstartTime >= LEDdelay*1000) {
      LEDstartTime=millis();

      if(lightreading < lowlight && LEDState==0 && ((hour < startsleep && hour > endsleep && quieth==true)||hour==-1) ){ // hour == -1 usecase for when wifi is not on
        Serial.println("Turning on LED");
        //N channel MOSFET for PCB(default off), RELAY for prototype(default ON)
        if(PCBEN==false){
          digitalWrite(LED,LOW);
        }else{
          digitalWrite(LED,HIGH);
        }
        LEDState=1;
        
      //LED is on, turn off led first and check ambient light sensor
      //If ambient light is high, dont check until next LEDdelay inverval
      //If ambient light is low, continue turning on LED
      }else if (LEDState==1){
        Serial.println("Testing ambient light sensor");
        if(PCBEN == false){
          digitalWrite(LED,HIGH);
          delay(100); //time for light sensor to react
        }else{
          digitalWrite(LED,LOW);
          delay(100); //time for the mosfet to discharge and light sensor to react
        }
        LEDState=0;
        
        #if defined(VEML7700)
          loopVEML7700();
        #endif
        #if defined(BH1750)
          loopBH1750();
        #endif

        if(lightreading < lowlight && LEDState==0 && ((hour < 20 && hour > 8)||hour==-1) ){
          Serial.println("Turning back on LED");
          if(PCBEN==false){
            digitalWrite(LED,LOW);
          }else{
            digitalWrite(LED,HIGH);
          }
          LEDState=1;
        }else{
          Serial.println("Turning off LED due to change in conditions");
          if(PCBEN==false){
            digitalWrite(LED,HIGH);
          }else{
            digitalWrite(LED,LOW);
          }
          
        }
      }
    }

  }else if (LEDmode == 1){
    // BrightnesArray[0]=BrightnesArray[1]; // Shift all readings down one position forgetting oldest reading
    // BrightnesArray[1]=BrightnessArray[2];
    // BrightnessArray[2] = lightreading; //Overwrite past reading with new reading
    // int SortedArray[3];
    // copy(begin(BrightnesArray), end(BrightnesArray), begin(SortedArray));
    // sort(SortedArray, SortedArray + 3);
    // Serial.print("Sorted Array: "); Serial.println(SortedArray);

  }else if(LEDmode ==3){
    Serial.println("LED Mode 3");
    if(lightreading < lowlight){
      Serial.println("Turning on LED");
      if(PCBEN==true){
        digitalWrite(LED,HIGH);
        Serial.println("LED SUCCESS");
      }else{
        digitalWrite(LED,LOW);
      }
    }else{
      Serial.println("Turning off LED");
      if(PCBEN==true){
        digitalWrite(LED,LOW);
      }else{
        digitalWrite(LED,HIGH);
      }
    }
  }

}



//placeholder function for light recommendation feature to check if there is enough light or not
void checksunlight(int analogpin, double desiredlight){
  if (analogRead(analogpin) < desiredlight && (hour > 18 or hour < 11)){
    Serial.println("Insufficient Light");
  }else{
    Serial.println("Sufficient Light");
  }
  //brightnesspercentage=analogRead(LDR)/LDRH;
}