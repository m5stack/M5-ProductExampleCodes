
#include <M5StickC.h>

HardwareSerial VSerial(1);

typedef struct
{
    int16_t dx;
    uint32_t area;
}v_response_t;


uint8_t I2CWrite1Byte(uint8_t Addr, uint8_t Data)
{
    Wire.beginTransmission(0x38);
    Wire.write(Addr);
    Wire.write(Data);
    return Wire.endTransmission();
}

uint8_t I2CWritebuff(uint8_t Addr, uint8_t *Data, uint16_t Length)
{
    Wire.beginTransmission(0x38);
    Wire.write(Addr);
    for (int i = 0; i < Length; i++)
    {
        Wire.write(Data[i]);
    }
    return Wire.endTransmission();
}

uint8_t Setspeed(int16_t Vtx, int16_t Vty, int16_t Wt)
{
    int16_t speed_buff[4] = {0};
    int8_t speed_sendbuff[4] = {0};

    Wt = (Wt > 100) ? 100 : Wt;
    Wt = (Wt < -100) ? -100 : Wt;

    Vtx = (Vtx > 100) ? 100 : Vtx;
    Vtx = (Vtx < -100) ? -100 : Vtx;
    Vty = (Vty > 100) ? 100 : Vty;
    Vty = (Vty < -100) ? -100 : Vty;

    Vtx = (Wt != 0) ? Vtx * (100 - abs(Wt)) / 100 : Vtx;
    Vty = (Wt != 0) ? Vty * (100 - abs(Wt)) / 100 : Vty;

    speed_buff[0] = Vty - Vtx - Wt;
    speed_buff[1] = Vty + Vtx + Wt;
    speed_buff[3] = Vty - Vtx + Wt;
    speed_buff[2] = Vty + Vtx - Wt;

    for (int i = 0; i < 4; i++)
    {
        speed_buff[i] = (speed_buff[i] > 100) ? 100 : speed_buff[i];
        speed_buff[i] = (speed_buff[i] < -100) ? -100 : speed_buff[i];
        speed_sendbuff[i] = speed_buff[i];
    }
    return I2CWritebuff(0x00, (uint8_t *)speed_sendbuff, 4);
}

void setup()
{
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(RED);

    VSerial.begin(115200, SERIAL_8N1, 33, 32);
    Wire.begin(0, 26);
}

enum
{
    kNoTarget = 0,
    kLeft,
    kRight,
    kStraight,
    kTooClose
};

const uint16_t kThreshold = 20; // If target is in range ±kThreshold, the car will go straight
v_response_t v_data;    // Data read back from V
uint8_t state = 0;  // Car's movement status

void loop()
{
    VSerial.write(0xAF);

    if(VSerial.available())
    {
        uint8_t buffer[5];
        VSerial.readBytes(buffer, 5);
        v_data.dx = (buffer[0] << 8) | buffer[1];
        v_data.area = (buffer[2] << 16) | (buffer[3] << 8) | buffer[4];

        if(v_data.dx > -120 && v_data.dx < 120)
        {
            if(v_data.area > 10000)
            {
                state = kTooClose;  // Stop
            }
            else if(v_data.dx > -kThreshold && v_data.dx < kThreshold)
            {
                state = kStraight;  // Go straight
            }
            else if(v_data.dx <= -kThreshold)
            {
                state = kLeft;  // Go left
            }
            else if(v_data.dx >= kThreshold)
            {
                state = kRight; // Go right
            }
            else
            {
                state = kNoTarget;  // Rotate
            }
            M5.Lcd.fillScreen(GREEN);
        }
        else
        {
            state = kNoTarget;  // Rotate
            M5.Lcd.fillScreen(RED);
        }

        Serial.printf("%d, %d, %d\n", v_data.dx, v_data.area, state);
    }

    //The speed and time here may need to be modified according to the actual situation
    switch(state)
    {
        case kNoTarget:
            Setspeed(0, 0, 20); //Duty ratio ±100
            delay(20);
            Setspeed(0, 0, 0);
        break;

        case kLeft:
            Setspeed(-20, 0, 0);
            delay(20);
            Setspeed(0, 0, 0);
        break;

        case kRight:
            Setspeed(20, 0, 0);
            delay(20);
            Setspeed(0, 0, 0);
        break;

        case kStraight:
            Setspeed(0, 20, 0);
            delay(20);
            Setspeed(0, 0, 0);
        break;

        case kTooClose:
            Setspeed(0, 0, 0);
        break;

    }
}