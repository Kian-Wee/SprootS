
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
        esp_deep_sleep_start();
    }

    //Adaptive deep sleep
    //Turns on often during the hot afternoon and less often at night


    //Automatic
    //Toggles between these modes depending on how much power is remaining
    //if(voltagesens)
    

}