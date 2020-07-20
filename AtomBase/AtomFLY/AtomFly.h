#ifndef __ATOMFLAY_H__
#define __ATOMFLAY_H__

#include <M5Atom.h>
#include <Adafruit_BMP280.h>

#define VL53L0X_REG_IDENTIFICATION_MODEL_ID 0xc0
#define VL53L0X_REG_IDENTIFICATION_REVISION_ID 0xc2
#define VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD 0x50
#define VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD 0x70
#define VL53L0X_REG_SYSRANGE_START 0x00
#define VL53L0X_REG_RESULT_INTERRUPT_STATUS 0x13
#define VL53L0X_REG_RESULT_RANGE_STATUS 0x14
#define TOF_ADDR 0x29

#define MPU6886_ADDRESS 0x69
#define MPU6886_WHOAMI 0x75
#define MPU6886_ACCEL_INTEL_CTRL 0x69
#define MPU6886_SMPLRT_DIV 0x19
#define MPU6886_INT_PIN_CFG 0x37
#define MPU6886_INT_ENABLE 0x38
#define MPU6886_ACCEL_XOUT_H 0x3B
#define MPU6886_ACCEL_XOUT_L 0x3C
#define MPU6886_ACCEL_YOUT_H 0x3D
#define MPU6886_ACCEL_YOUT_L 0x3E
#define MPU6886_ACCEL_ZOUT_H 0x3F
#define MPU6886_ACCEL_ZOUT_L 0x40

#define MPU6886_TEMP_OUT_H 0x41
#define MPU6886_TEMP_OUT_L 0x42

#define MPU6886_GYRO_XOUT_H 0x43
#define MPU6886_GYRO_XOUT_L 0x44
#define MPU6886_GYRO_YOUT_H 0x45
#define MPU6886_GYRO_YOUT_L 0x46
#define MPU6886_GYRO_ZOUT_H 0x47
#define MPU6886_GYRO_ZOUT_L 0x48

#define MPU6886_USER_CTRL 0x6A
#define MPU6886_PWR_MGMT_1 0x6B
#define MPU6886_PWR_MGMT_2 0x6C
#define MPU6886_CONFIG 0x1A
#define MPU6886_GYRO_CONFIG 0x1B
#define MPU6886_ACCEL_CONFIG 0x1C
#define MPU6886_ACCEL_CONFIG2 0x1D
#define MPU6886_FIFO_EN 0x23

//#define G (9.8)
#define RtA 57.324841
#define AtR 0.0174533
#define Gyro_Gr 0.0010653

class AtomFly
{
public:
    AtomFly(/* args */);
    ~AtomFly();
    void begin(void);
    int initFly(void);

public:
    enum Ascale
    {
        AFS_2G = 0,
        AFS_4G,
        AFS_8G,
        AFS_16G
    };

    enum Gscale
    {
        GFS_250DPS = 0,
        GFS_500DPS,
        GFS_1000DPS,
        GFS_2000DPS
    };

    enum Motor
    {
        kMotor_A = 0,
        kMotor_B,
        kMotor_C,
        kMotor_D 
    };

    Gscale Gyscale = GFS_2000DPS;
    Ascale Acscale = AFS_8G;

    void getAccelAdc(int16_t *ax, int16_t *ay, int16_t *az);
    void getGyroAdc(int16_t *gx, int16_t *gy, int16_t *gz);
    void getTempAdc(int16_t *t);
    void getAccelData(float *ax, float *ay, float *az);
    void getGyroData(float *gx, float *gy, float *gz);
    void getTempData(float *t);
    void SetGyroFsr(Gscale scale);
    void SetAccelFsr(Ascale scale);
    void getAhrsData(float *pitch, float *roll, float *yaw);
    void getAttitude(double *pitch, double *roll);

    void WritePWM(uint8_t,uint8_t);
    void WriteAllPWM(uint8_t);

    uint16_t readTOF();
private:
    uint8_t _PWMPinMap[4] = {22, 19, 23, 33};
    Adafruit_BMP280 *_bmp; // use I2C interface

    float aRes, gRes;
    byte _gbuf[16];
    float _last_theta = 0;
    float _last_phi = 0;
    float _alpha = 0.5;
    void I2C_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *read_Buffer);
    void I2C_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *write_Buffer);
    void getGres();
    void getAres();


    uint16_t bswap(byte b[]);
    uint16_t makeuint16(int lsb, int msb);

    void write_byte_data(byte data);
    void write_byte_data_at(byte reg, byte data);
    void write_word_data_at(byte reg, uint16_t data);
    byte read_byte_data();
    byte read_byte_data_at(byte reg);
    uint16_t read_word_data_at(byte reg);
    void read_block_data_at(byte reg, int sz);
    uint16_t VL53L0X_decode_vcsel_period(short vcsel_period_reg);
};

extern AtomFly fly;

#endif
