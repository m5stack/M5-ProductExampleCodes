[![Gitter](https://badges.gitter.im/Join%20Chat.svg)]




ESPHap
===========

This is a library for easily & efficiently integrating arduino projects based on ESP32/ESP8266 with Apple Home Kit by  native protocols 
and without any additional bridges

At this moment  ESP32 supported and tested well.
ESP8266 is ported as well , at this moment under the testing stage, Hovewer there is some minor bugs and especially in the pairing process. Not all of them finished smooth. Hovewer it can be maximum 3 attempts (in my experience) and after that everything working fine. I'd say that after pairing (once operation) devices works well and stable

Many thanks to [maximkulkin](https://github.com/maximkulkin) for providing fine libraries for native integration,
this project uses this as well.

Many thanks to [Mixiaoxiao](https://github.com/Mixiaoxiao)  to give a hint for ESP8266 to stop watchdog and some functions with lower memory usage

## Build instruction

This library should be located under standart Arduino libraries folder. 
Library is depends from [wolfssl](https://github.com/wolfSSL)  , but it requires some preparation before usage,
full instruction how to do that, can be found [there](https://www.wolfssl.com/doxygen/md__Users_alexabrahamson_Work_wolfssl-CLEAN_IDE_ARDUINO_README.html)

if you are doing this manually you should made specific configuration before usage.
For simplify process you can use [wolfssl.rar](https://github.com/Yurik72/ESPHap/blob/master/wolfssl/wolfSSL.rar) archive , which already prepared. You just need to extract this content  into Arduino Libraries folder

If you are going to prepare this manually, please reuse/check settings.h and user_settings.h  from attached archive or
 [this location](https://github.com/Yurik72/ESPHap/tree/master/wolfssl)
 You need copy/replace this files in wolfssl components.


! Note library as well working well with wolfssl 3.13.0. For ESP8266 it's MUST. Please extract wolfSSL_3_13_0.rar
into  Arduino Libraries folder. user_setting.h and setting.h MUST be used from this Archive

This library was tested in the following enviropment:

ESP32 board library  version 1.0.4

ESP8266 board library  version 2.6.3

Arduino version 1.8.12

## Getting help


## Simple example

examples folder contains very simple examples EspHapLed (for ESP32) and EspHapLed8266 (for ESP8266)how to handle led (or any such as relay) 
More examples will be later, hovewer you can see the same examples/implemenmtation
[esphapcontroller](https://github.com/Yurik72/esphapcontroller)

This example used as well implemenation for pairing storage. it will be stored in spiffs system with file name pair.dat

In the ino file change your wifi name and password. After connecting to wifi device should be accesible for pairing from Apple home.
At this moment QR is not generated, so please use manual pairing by enetering password  11111111

More detail instruction can be found [there](https://www.instructables.com/id/Arduino-With-ESP32-and-Native-Apple-HomeKit-Integr/)



## Sonoff example

Example folders contains sketch for Sonoff. As mentioned ESP8266 in process of testing (Sonoff is esp8265 )
But example already works quite fine. Small problem detected during the pairing. But after that works well

Please have a look [instructions](https://github.com/Yurik72/ESPHap/wiki/Build-Sonoff-Basic)

## Thermostat example

Example folders contains sketch for [Thermostat DHT](https://github.com/Yurik72/ESPHap/tree/master/examples/EspHap_DHT11), 
[Universal Thermostat] (https://github.com/Yurik72/ESPHap/tree/master/examples/EspHap_Thermostat)which shows on Apple Home two icons Temperathure and Humidity. There is universal sketch applicable for ESP32 and ESP8266 and using DHT sensor, BME28 or Dallas, 
as well any other hardware can be easily implemented
Hovewer any sensor can be used with simple code changes...
Those sketch as well includes advanced feathures: Web File Manager, OTA, Simple web site. For the [Universal Thermostat] (https://github.com/Yurik72/ESPHap/tree/master/examples/EspHap_Thermostat) it's example how to send historical data to [thingSpeak](https://thingspeak.com/)

Build instruction the same as for sketches avove.

## Switch example

Example folders contains sketch for [Switch](https://github.com/Yurik72/ESPHap/tree/master/examples/EspHap_Switch), which shows on Apple simple Switch icon. There is universal sketch applicable for ESP32 and ESP8266 and any relay can be used to manage external devices.

Those sketch as well includes advanced feathures: Web File Manager, OTA, Simple web site.

Build instruction the same as for sketches avove.

## RGB Strip (WS2812) & Motion example

Example folders contains sketch for [RGB & Motion devices](https://github.com/Yurik72/ESPHap/tree/master/examples/EspHap_RGB_Motion), which shows on Apple Home two icons: RGB Bulb  and Motion Sensor. There is universal sketch applicable for ESP32 and ESP8266 and using WS2812 Led strip.
Those sketch as well includes advanced feathures: Web File Manager, OTA, Simple web site and allows to demonstrate  two direction for the Apple Home Kit. Means when RGB (Brightness , Color ,State ) is changed from the built-in web site, Apple Home Kit refreshes the RGB bulb state.
For the motion sensor can be used anyone, skecth simples reads HIGH value for the predefined GPIO. For instance HC-SR501 can be used.
As well for RGB can be used any strip, the question is how to proceed with Brightness and Color values received from the Apple.

Those combination potentially has good demonstration for Apple Home Automation, based on the Motion state RGB can be switched ON/OFF...

Build instruction the same as for sketches avove.

## Button example

Example folders contains sketch for [Button](https://github.com/Yurik72/ESPHap/tree/master/examples/EspHap_Button), which shows on Apple simple Button icon. There is universal sketch applicable for ESP32 and ESP8266 (Testing in progress on ESP8266) and any Button scenarios on Appllle  can be used to manage automation.

Those sketch as well includes advanced feathures: Web File Manager, OTA, Simple web site.

Build instruction the same as for sketches avove.

## Air Quality sensor example

Example folders contains sketch for [ Air Quality sensor](https://github.com/Yurik72/ESPHap/tree/master/examples/EspHap_AirQuality_MQ135), which shows on Apple Air quality Sensor icon.
Sketch is designed with usage for MQ135 sensor. There is universal sketch applicable for ESP32 and ESP8266. Main advantage that build-in web site shows history trends of PPM level of dioxide.  
Code is contains comments and one of the important thing for this sketch is calibration based on your real sensor, for this purpose following line should be changed 

const float factor=14.0;//to be calibrated with your MQ135


Those sketch as well includes advanced feathures: Web File Manager, OTA, Simple web site.

Build instruction the same as for sketches avove.


## Are you interesting to support this project ?

You can easilly do that by donations

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=JVZWJ6FSMURSL&currency_code=USD&source=url)






*TODO* - 
