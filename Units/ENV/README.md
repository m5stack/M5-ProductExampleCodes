# Example for ENV Unit

*注意，编译例程前，需要将DHT12.h和DHT12.cpp文件放置在例程ENV.ino所在文件夹下，并且通过Arduino IDE的库管理安装[Adafruit BMP280](https://github.com/adafruit/Adafruit_BMP280_Library)库，然后下载这个[地址](https://github.com/adafruit/Adafruit_Sensor)的Adafruit_Sensor.h，并放置在Adafruit_BMP280_Library文件夹中。*

## Desc

从ENV Unit读取温湿度和压力值，然后通过串口0打印到串口终端，同时在M5Core的屏幕上显示这些数据。

## Result

串口终端显示结果：

Temperatura: 25.40*C  Humedad: 28.50%  Pressure: 102137.55Pa

Temperatura: 25.40*C  Humedad: 28.50%  Pressure: 102136.17Pa

Temperatura: 25.50*C  Humedad: 28.50%  Pressure: 102139.25Pa

Temperatura: 25.50*C  Humedad: 28.50%  Pressure: 102133.55Pa

Temperatura: 25.50*C  Humedad: 28.50%  Pressure: 102137.08Pa

Temperatura: 25.50*C  Humedad: 28.50%  Pressure: 102140.25Pa




