#include <sensor.h>
  
//for wire library, use Wire.pins(SDA,SCL);
//for two wire library, change the wire.begin(SDA,SCL)

#if defined(VEML7700)

#include "DFRobot_VEML7700.h"
DFRobot_VEML7700 als;

void setupVEML7700(){
  als.begin();
}

void loopVEML7700(){

  float lux;
  
  als.getALSLux(lux);
  Serial.print("Lux:");
  Serial.print(lux);
  Serial.println(" lx");

  lightreading=lux;

}

#endif


#if defined(BH1750)

#include <hp_BH1750.h>
hp_BH1750 BH; //???For some reason using object name BH1750 yields definiton errors

void setupBH1750() {
  // put your setup code here, to run once:
  BH.begin(BH1750_TO_GROUND);//  change to BH1750_TO_VCC if address pin is connected to VCC.
  BH.calibrateTiming();
}

void loopBH1750() {
  // put your main code here, to run repeatedly:
  BH.start(BH1750_QUALITY_LOW, 31);    //  starts a measurement at low quality
  // Serial.print(BH.getLux());           //  do a blocking read and waits until a result receives
  // Serial.print(char(9));
  BH.start(BH1750_QUALITY_HIGH2, 254); //  starts a measurement with high quality but restricted range in brightness
  float val = BH.getLux();
  // Serial.print(val);
  // Serial.print(char(9));
  if (BH.saturated() == true){
    val = val * BH.getMtregTime() / BH.getTime();  //  here we calculate from a saturated sensor the brightness!
  }
  lightreading=val;
  Serial.print("BH1750 Reading:"); Serial.print(val); Serial.println("");
}

#endif



#if defined(BME680)

#include "Zanshin_BME680.h"  // Library from https://github.com/Zanduino/BME680
/**************************************************************************************************
** Declare all program constants                                                                 **
**************************************************************************************************/
const uint32_t SERIAL_SPEED{115200};  ///< Set the baud rate for Serial I/O

/**************************************************************************************************
** Declare global variables and instantiate classes                                              **
**************************************************************************************************/
BME680_Class BME;  ///< Create an instance of the BME680 class
///< Forward function declaration with default value for sea level
float altitude(const int32_t press, const float seaLevel = 1013.25);
float altitude(const int32_t press, const float seaLevel) {
  /*!
  @brief     This converts a pressure measurement into a height in meters
  @details   The corrected sea-level pressure can be passed into the function if it is known,
             otherwise the standard atmospheric pressure of 1013.25hPa is used (see
             https://en.wikipedia.org/wiki/Atmospheric_pressure) for details.
  @param[in] press    Pressure reading from BME680
  @param[in] seaLevel Sea-Level pressure in millibars
  @return    floating point altitude in meters.
  */
  static float Altitude;
  Altitude =
      44330.0 * (1.0 - pow(((float)press / 100.0) / seaLevel, 0.1903));  // Convert into meters
  return (Altitude);
}  // of method altitude()

void setupBME680() {
  /*!
  @brief    Arduino method called once at startup to initialize the system
  @details  This is an Arduino IDE method which is called first upon boot or restart. It is only
            called one time and then control goes to the main "loop()" method, from which
            control never returns
  @return   void
  */
//   Serial.begin(SERIAL_SPEED);  // Start serial port at Baud rate
// #ifdef __AVR_ATmega32U4__      // If this is a 32U4 processor, then wait 3 seconds to init USB port
//   delay(3000);
// #endif
  //Serial.print(F("Starting I2CDemo example program for BME680\n"));
  //Serial.print(F("- Initializing BME680 sensor\n"));
  // while (!BME.begin(I2C_STANDARD_MODE)) {  // Start BME680 using I2C, use first device found
  //   Serial.print(F("-  Unable to find BME680. Trying again in 5 seconds.\n"));
  //   delay(5000);
  // }  // of loop until device is located

  BME.begin(I2C_STANDARD_MODE); // Replace sensor checking loop

  //Serial.print(F("- Setting 16x oversampling for all sensors\n"));
  BME.setOversampling(TemperatureSensor, Oversample16);  // Use enumerated type values
  BME.setOversampling(HumiditySensor, Oversample16);     // Use enumerated type values
  BME.setOversampling(PressureSensor, Oversample16);     // Use enumerated type values
  //Serial.print(F("- Setting IIR filter to a value of 4 samples\n"));
  BME.setIIRFilter(IIR4);  // Use enumerated type values
  //Serial.print(F("- Setting gas measurement to 320\xC2\xB0\x43 for 150ms\n"));  // "�C" symbols
  BME.setGas(320, 150);  // 320�c for 150 milliseconds
}  // of method setup()

void loopBME680() {
  /*!
  @brief    Arduino method for the main program loop
  @details  This is the main program for the Arduino IDE, it is an infinite loop and keeps on
            repeating. The "sprintf()" function is to pretty-print the values, since floating
            point is not supported on the Arduino, split the values into those before and those
            after the decimal point.
  @return   void
  */
  static int32_t  temp, humidity, pressure, gas;  // BME readings
  static char     buf[16];                        // sprintf text buffer
  static float    alt;                            // Temporary variable
  static uint16_t loopCounter = 0;                // Display iterations
  if (loopCounter % 25 == 0) {                    // Show header @25 loops
    Serial.print(F("\nLoop Temp\xC2\xB0\x43 Humid% Press hPa   Alt m Air m"));
    Serial.print(F("\xE2\x84\xA6\n==== ====== ====== ========= ======= ======\n"));  // "�C" symbol
  }                                                     // if-then time to show headers
  BME.getSensorData(temp, humidity, pressure, gas);  // Get readings
  // if (loopCounter++ != 0) {                             // Ignore first reading, might be incorrect
    sprintf(buf, "%4d %3d.%02d", (loopCounter - 1) % 9999,  // Clamp to 9999,
            (int8_t)(temp / 100), (uint8_t)(temp % 100));   // Temp in decidegrees
    //Serial.print("Temperature: ");
    tempreading=double(temp/100);
    Serial.print(buf);
    //double tempreading=double(buf);
    sprintf(buf, "%3d.%03d", (int8_t)(humidity / 1000),
            (uint16_t)(humidity % 1000));  // Humidity milli-pct
    //Serial.print("; Humidity: ");
    humidityreading=double(humidity/1000);
    Serial.print(buf);
    //double humidityreading=double(buf);
    sprintf(buf, "%7d.%02d", (int16_t)(pressure / 100),
            (uint8_t)(pressure % 100));  // Pressure Pascals
    //Serial.print("; Pressure: ");
    pressurereading=double(pressure/100);
    Serial.print(buf);
    //double pressurereading=double(buf);
    alt = altitude(pressure);                                                // temp altitude
    sprintf(buf, "%5d.%02d", (int16_t)(alt), ((uint8_t)(alt * 100) % 100));  // Altitude meters
    //Serial.print("; Altitude: ");
    Serial.print(buf);
    sprintf(buf, "%4d.%02d\n", (int16_t)(gas / 100), (uint8_t)(gas % 100));  // Resistance milliohms
    //Serial.print("; Air Resistance: ");
    Serial.print(buf);

  // }                // of ignore first reading
  //Wire.endTransmission();
}  // of method loop()

#endif

#if defined(BME280)

//Library from Adafruit
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // use I2C interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

void setupBME280() {

  Serial.println(F("BME280 Sensor event test"));

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (1) delay(10);
  }
  
  bme_temp->printSensorDetails();
  bme_pressure->printSensorDetails();
  bme_humidity->printSensorDetails();
}

void loopBME280() {
  sensors_event_t temp_event, pressure_event, humidity_event;
  bme_temp->getEvent(&temp_event);
  bme_pressure->getEvent(&pressure_event);
  bme_humidity->getEvent(&humidity_event);
  
  Serial.print(F("Temperature = "));
  Serial.print(temp_event.temperature);
  Serial.println(" *C");
  tempreading=temp_event.temperature;

  Serial.print(F("Humidity = "));
  Serial.print(humidity_event.relative_humidity);
  Serial.println(" %");
  humidityreading=temp_event.temperature;

  Serial.print(F("Pressure = "));
  Serial.print(pressure_event.pressure);
  Serial.println(" hPa");
  tempreading=temp_event.temperature;

  Serial.println();
}
#endif



#if defined(BMP280)

// Library from Adafruit
#include <Adafruit_BMP280.h>

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

void setupBMP280() {
  Serial.println(F("BMP280 setup"));
  bmp.begin(0x76); // Some cheaper BMP280 sensors have this address
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loopBMP280() {
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    tempreading=bmp.readTemperature();

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    pressurereading=bmp.readPressure();

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1010)); // Adjusted to local forecast!, atmospheric pressure (at sea level)
    Serial.println(" m");

    Serial.println();
}
#endif