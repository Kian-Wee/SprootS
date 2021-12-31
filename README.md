# SprootS(WIP)

SprootS is a standalone automated low cost solution for horticulture. To keep costs low, it uses off the shelf sensors and irrigation components and is powered by the ESP32.

Currently, the web portal may only be accessed on a local area network for security but a telegram bot is avaliable for remote use of certain commands.

##  Software Installation
Use Visual Studio Code and the PlatformIO plugin to flash the firmware and upload the filesystem image(for webpage). The arduino core for esp32 is also required.

### Libraries
The following libraries are required.
[Arduino-ESP32*( LGPL-2.1)*](https://github.com/espressif/arduino-esp32)
[DFRobot_VEML7700-master*(MIT)*](https://github.com/DFRobot/DFRobot_VEML7700) (if required)
[BME680*(GPL-3.0)*](https://github.com/Zanduino/BME680) (if required)
[Adafruit BME280 Library](https://github.com/adafruit/Adafruit_BME280_Library) (if required)
[Adafruit BMP280 Library](https://github.com/adafruit/Adafruit_BMP280_Library) (if required)
[ArduinoJson*(MIT)*](https://github.com/bblanchon/ArduinoJson)
[Autoconnect*(MIT)*](https://github.com/Hieromon/AutoConnect) (if a captive wifi setup webpage is required)
[hp_BH1750*(MIT)*](https://github.com/Starmbi/hp_BH1750)
[Int64String*(MIT)*](https://github.com/djGrrr/Int64String)
[UniversalTelegramBot*(MIT)*](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot)

### HTML/JS Libraries
(These are included in the web portal and automatically downloaded via CDN)
[Bootstrap*(MIT)*](https://getbootstrap.com/)
[Chart.js*(MIT)*](https://www.chartjs.org/)

### Code Setup
In the ```main.cpp``` file, uncomment and enter in your network ssid and password. The other settings may be left unchanged. The ```config.h``` file provides other settings such as pinout definitions and the sensors in use.

### Telegram Bot
Fill in the ```BOTtoken``` and ```CHAT_ID``` parameters by creating a telegram bot using @BotFather and find out the chatid with @IDBot.

## Sensors
SprootS is designed for use with cheap off the shelf sensors to keep costs down. These three broad ranges of sensors are used.
- a light sensor to determine if the plant has recieved sufficient light
- a moisture/water level(WIP) sensor to check soil moisture or water level for hydroponics
- an environment sensor to provide additional data such as temperature, air pressure and humidity
All of these sensors may be purchased on aliexpress.

### Currently supported sensors and procurement
#### Light Sensors
**BH1750** (from mouser, cheaper on aliexpress)
VEML7700

#### Environment Sensors
**BMP280**([cheaper BME280 on aliexpress are usually BMP280](https://goughlui.com/2018/08/05/note-bosch-sensortec-bmp280-vs-bme280-sensor-confusion/), but it performs sufficiently well)
BME280 (Measures Humidity in adition to BMP280)
BME680 (more accurate and costly than the BME280)

#### Soil Moisture Sensors
*Do note that calibration is required for different soil types*
**Capacitive Soil Moisture sensor**([sample listing](https://www.aliexpress.com/item/4001131897353.html?spm=a2g0o.productlist.0.0.4e6b4b8506mfOR&algo_pvid=2f884ae7-8af6-4514-9e48-394b6093bfcd&algo_exp_id=2f884ae7-8af6-4514-9e48-394b6093bfcd-1&pdp_ext_f=%7B%22sku_id%22%3A%2210000014716905676%22%7D))

### Sensor configuration

## Other Hardware
For automatic irrigation, dc water pumps may be used along with silicone/irrigation tubing. Solar panels may also aid in supplying energy when used permernantly outdoors.

## Sample Hardware(WIP)
The following setup uses a 3D printed enclosure along with the fireebeetle32 and a custom pcb for semi-outdoor use.

### 18650 Batteries
To lower costs and recycle, it is highly recommended to reuse 18650 cells from old laptop and devices(if you are capable of disassembling it safely). Do note that old batteries(even from brands such as samsung) may suffer from a significant voltage drop when under load from the water pump which could trigger the brownout detector and cause the esp32 to restart(causing the pump to pulse on and off). This was not an issue with brand new panasonic 18650 cells.

Future iterations to the PCB may include capacitors to fix this issue.

### Solar panel
TBC