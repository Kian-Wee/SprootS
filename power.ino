//TODO: FIGURE OUT WHY Long duration(>40min) causes it to reset
// Main power function for either individual selection of power saving mode or automatic
bool powermain(int mode){

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
        esp_sleep_enable_timer_wakeup(sleepduration * 1000000); //convert from ns to s
        Serial.flush(); //To get rid of deep sleep reset error, esp32 needs time to shutdown all messages
        esp_deep_sleep_start();
    }

    //Adaptive deep sleep
    //Turns on often during the hot afternoon and less often at night


    //Automatic
    //Toggles between these modes depending on how much power is remaining
    //if(voltagesens)
    

}

extern bool Touched;
extern bool justwoke;
extern int touchtime;
extern int touchduration;

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