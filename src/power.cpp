#include "power.h"

// Main power function for either individual selection of power saving mode or automatic
bool powermain(int mode,int sleepduration){

    //No power saving
    if (mode==0){
        return true;       
    }

    //Perodic Deep Sleep
    //Sleeps at every interval
    else if(mode==1){

        // Wire.end(); // shutdown/power off I2C hardware, 
        pinMode(SDA,INPUT); // needed because Wire.end() enables pullups, power Saving
        pinMode(SCL,INPUT);

        Serial.printf("Sleeping for: %d m and %d s", int(sleepduration/60),sleepduration%60);
        uint64_t totalsleepduration = uint64_t(sleepduration) * 1000000; // Needed to store large variable if not it exceeds the allowed size of int and turns negative
        esp_sleep_enable_timer_wakeup(totalsleepduration); //convert from ns to s
        Serial.flush(); //To get rid of deep sleep reset error, esp32 needs time to shutdown all messages
        esp_deep_sleep_start();
    }

    //Adaptive deep sleep
    //Turns on often during the hot afternoon and less often at night, ESP32 is also underclocked from 240mHz to 80mHz
    else if (mode==2){
      setCpuFrequencyMhz(80);
    }

    //Automatic
    //Toggles between these modes depending on how much power is remaining
    //if(voltagesens)
    

}



//simple function to caculate battery level
float caculatebatterylevel(int analogpin){
  float R1 = 470000.0; //Value of voltage divider resistors(in ohm)
  float R2 = 470000.0;
  float voltage_value = (analogRead(analogpin) * 3.3 ) / (4095);
  voltage_value = voltage_value / (R2/(R1+R2));
  Serial.print("Voltage level is: "); Serial.print(voltage_value);
  if(voltage_value==0){
    Serial.print(". Battery percentage is: "); Serial.println(0);
    return 0;
  }else{
    Serial.print(". Battery percentage is: "); Serial.println((voltage_value-3.3)/(4.2-3.3)*100);
    return (voltage_value-3.3)/(4.2-3.3)*100; //convert battery level to percentage
  }
}



void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : 
    Serial.printf("Wakeup caused by touchpad, setting standby timer for %d seconds\n",touchduration);
    Touched = true;
    touchtime=millis();
    justwoke = false;
    break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void callback(){
  //placeholder callback function for touch wakeup
  Serial.println("TOUCH WAKEUP CALLBACK");
}