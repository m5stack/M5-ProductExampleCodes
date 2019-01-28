#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "MPU9250.h"
#include "quaternionFilters.h"


#define AHRS true         // Set to false for basic data read
#define SerialDebugMPU9250 false  // Set to true to get Serial output for debugging
#define SerialDebugCalibrate false  // Set to true to get Serial output for debugging

MPU9250 IMU;


void setupMPU9250() {
  byte c = IMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  Serial.print("MPU9250 "); Serial.print("I AM "); Serial.print(c, HEX);
  Serial.print(" I should be "); Serial.println(0x71, HEX);
  if (c == 0x71) {
    Serial.println("MPU9250 is online...");
    // Start by performing self test and reporting values
    IMU.MPU9250SelfTest(IMU.SelfTest);
    if (SerialDebugCalibrate) {
      Serial.print("x-axis self test: acceleration trim within : ");
      Serial.print(IMU.SelfTest[0], 1); Serial.println("% of factory value");
      Serial.print("y-axis self test: acceleration trim within : ");
      Serial.print(IMU.SelfTest[1], 1); Serial.println("% of factory value");
      Serial.print("z-axis self test: acceleration trim within : ");
      Serial.print(IMU.SelfTest[2], 1); Serial.println("% of factory value");
      Serial.print("x-axis self test: gyration trim within : ");
      Serial.print(IMU.SelfTest[3], 1); Serial.println("% of factory value");
      Serial.print("y-axis self test: gyration trim within : ");
      Serial.print(IMU.SelfTest[4], 1); Serial.println("% of factory value");
      Serial.print("z-axis self test: gyration trim within : ");
      Serial.print(IMU.SelfTest[5], 1); Serial.println("% of factory value");
    }
    Serial.println("MPU9250 acceleration and gyration self test done!");

    // Calibrate gyro and accelerometers, load biases in bias registers
    IMU.calibrateMPU9250(IMU.gyroBias, IMU.accelBias);

    IMU.initMPU9250();
    // Initialize device for active mode read of acclerometer, gyroscope, and
    // temperature
    Serial.println("MPU9250 initialized for active data mode....");

    // Read the WHO_AM_I register of the magnetometer, this is a good test of
    // communication
    byte d = IMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    Serial.print("AK8963 "); Serial.print("I AM "); Serial.print(d, HEX);
    Serial.print(" I should be "); Serial.println(0x48, HEX);

    // Get magnetometer calibration from AK8963 ROM
    IMU.initAK8963(IMU.magCalibration);
    // Initialize device for active mode read of magnetometer
    Serial.println("AK8963 initialized for active data mode....");
    if (SerialDebugCalibrate) {
      Serial.println("Calibration values: ");
      Serial.print("X-Axis sensitivity adjustment value ");
      Serial.println(IMU.magCalibration[0], 2);
      Serial.print("Y-Axis sensitivity adjustment value ");
      Serial.println(IMU.magCalibration[1], 2);
      Serial.print("Z-Axis sensitivity adjustment value ");
      Serial.println(IMU.magCalibration[2], 2);
    }
    Serial.println("AK8963 magCalibration done!");
  }
}

void readMPU9250() {
  // If intPin goes high, all data registers have new data
  // On interrupt, check if data ready interrupt
  if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {
    IMU.readAccelData(IMU.accelCount);  // Read the x/y/z adc values
    IMU.getAres();

    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    IMU.ax = (float)IMU.accelCount[0] * IMU.aRes; // - accelBias[0];
    IMU.ay = (float)IMU.accelCount[1] * IMU.aRes; // - accelBias[1];
    IMU.az = (float)IMU.accelCount[2] * IMU.aRes; // - accelBias[2];

    IMU.readGyroData(IMU.gyroCount);  // Read the x/y/z adc values
    IMU.getGres();

    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    IMU.gx = (float)IMU.gyroCount[0] * IMU.gRes;
    IMU.gy = (float)IMU.gyroCount[1] * IMU.gRes;
    IMU.gz = (float)IMU.gyroCount[2] * IMU.gRes;

    IMU.readMagData(IMU.magCount);  // Read the x/y/z adc values
    IMU.getMres();
    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    IMU.magbias[0] = +470.;
    // User environmental x-axis correction in milliGauss TODO axis??
    IMU.magbias[1] = +120.;
    // User environmental x-axis correction in milliGauss
    IMU.magbias[2] = +125.;

    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    IMU.mx = (float)IMU.magCount[0] * IMU.mRes * IMU.magCalibration[0] -
             IMU.magbias[0];
    IMU.my = (float)IMU.magCount[1] * IMU.mRes * IMU.magCalibration[1] -
             IMU.magbias[1];
    IMU.mz = (float)IMU.magCount[2] * IMU.mRes * IMU.magCalibration[2] -
             IMU.magbias[2];
  } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

  // Must be called before updating quaternions!
  IMU.updateTime();

  // Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
  // the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
  // (+ up) of accelerometer and gyro! We have to make some allowance for this
  // orientationmismatch in feeding the output to the quaternion filter. For the
  // MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
  // along the x-axis just like in the LSM9DS0 sensor. This rotation can be
  // modified to allow any convenient orientation convention. This is ok by
  // aircraft orientation standards! Pass gyro rate as rad/s
  //  MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f,  my,  mx, mz);
  MahonyQuaternionUpdate(IMU.ax, IMU.ay, IMU.az, IMU.gx * DEG_TO_RAD,
                         IMU.gy * DEG_TO_RAD, IMU.gz * DEG_TO_RAD, IMU.my,
                         IMU.mx, IMU.mz, IMU.deltat);
  // Serial print and/or display at 0.5 s rate independent of data rates
  IMU.delt_t = millis() - IMU.count;

  if (IMU.delt_t > 20) {
    if (SerialDebugMPU9250) {
      Serial.print("ax = ");
      Serial.print((int)1000 * IMU.ax);
      Serial.print(" ay = ");
      Serial.print((int)1000 * IMU.ay);
      Serial.print(" az = ");
      Serial.print((int)1000 * IMU.az);
      Serial.println(" mg");

      Serial.print("gx = ");
      Serial.print( IMU.gx, 2);
      Serial.print(" gy = ");
      Serial.print( IMU.gy, 2);
      Serial.print(" gz = ");
      Serial.print( IMU.gz, 2);
      Serial.println(" deg/s");

      Serial.print("mx = ");
      Serial.print( (int)IMU.mx );
      Serial.print(" my = ");
      Serial.print( (int)IMU.my );
      Serial.print(" mz = ");
      Serial.print( (int)IMU.mz );
      Serial.println(" mG");

      //Serial.print("q0 = ");
      Serial.print(*getQ());
      // Serial.print(" qx = ");
      Serial.print(*(getQ() + 1));
      // Serial.print(" qy = ");
      Serial.print(*(getQ() + 2));
      // Serial.print(" qz = ");
      Serial.println(*(getQ() + 3));
    }

    IMU.yaw   = atan2(2.0f * (*(getQ() + 1) * *(getQ() + 2) + *getQ() *
                              *(getQ() + 3)), *getQ() * *getQ() + * (getQ() + 1) * *(getQ() + 1)
                      - * (getQ() + 2) * *(getQ() + 2) - * (getQ() + 3) * *(getQ() + 3));
    IMU.pitch = -asin(2.0f * (*(getQ() + 1) * *(getQ() + 3) - *getQ() *
                              *(getQ() + 2)));
    IMU.roll  = atan2(2.0f * (*getQ() * *(getQ() + 1) + * (getQ() + 2) *
                              *(getQ() + 3)), *getQ() * *getQ() - * (getQ() + 1) * *(getQ() + 1)
                      - * (getQ() + 2) * *(getQ() + 2) + * (getQ() + 3) * *(getQ() + 3));
    IMU.pitch *= RAD_TO_DEG;
    IMU.yaw   *= RAD_TO_DEG;
    // Declination of SparkFun Electronics (40°05'26.6"N 105°11'05.9"W) is
    //  8° 30' E  ± 0° 21' (or 8.5°) on 2016-07-19
    // - http://www.ngdc.noaa.gov/geomag-web/#declination
    IMU.yaw   -= 8.5;
    IMU.roll  *= RAD_TO_DEG;

    if (SerialDebugMPU9250) {
      // Serial.println("Yaw, Pitch, Roll: ");
      Serial.print(IMU.yaw, 2);
      Serial.print(", ");
      Serial.print(IMU.pitch, 2);
      Serial.print(", ");
      Serial.println(IMU.roll, 2);
    }

    IMU.count = millis();
    IMU.sumCount = 0;
    IMU.sum = 0;

  } // if (IMU.delt_t > 20)
}
#endif // _SENSOR_H_
