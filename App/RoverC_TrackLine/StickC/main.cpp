
#include <M5StickC.h>
#include <math.h>

HardwareSerial VSerial(1);
TFT_eSprite tft = TFT_eSprite(&M5.Lcd);

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
    M5.Lcd.setRotation(0);
    M5.Lcd.fillScreen(0);

    tft.setColorDepth(8);
    tft.createSprite(80, 160);
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);

    VSerial.begin(115200, SERIAL_8N1, 33, 32);
    Wire.begin(0, 26);

    Setspeed(0, 0, 0);

    VSerial.write(0xAF);
}
float rho;
float theta;
int16_t ux;
int16_t uw;

#define BASE_SPEED  20
void loop()
{
    M5.update();
    if(M5.BtnA.wasReleased())
    {
        Setspeed(0, 0, 0);
        ESP.restart();
    }

    if(VSerial.available())
    {
        VSerial.write(0xAF);
        tft.fillSprite(0);

        uint8_t b_rho[4];
        uint8_t b_theta[4];
        uint8_t b_rx[6];
        uint8_t b_lx[6];
        uint8_t b_cx[6];
        VSerial.readBytes(b_rho, 4);
        VSerial.readBytes(b_theta, 4);
        VSerial.readBytes(b_rx, 6);
        VSerial.readBytes(b_lx, 6);
        VSerial.readBytes(b_cx, 6);

        int y = 80;
        for (int i = 0; i < 6; i++)
        {
            tft.drawLine(b_cx[i], y, b_cx[i+1], y - 16, TFT_GREEN);
            tft.drawLine(b_rx[i], y, b_rx[i+1], y - 16, TFT_WHITE);
            tft.drawLine(b_lx[i], y, b_lx[i+1], y - 16, TFT_WHITE);
            y -= 16;
        }

        rho = *((float*)b_rho);
        theta = *((float*)b_theta);

        uw = (int16_t)theta;
        ux = (int16_t)rho;

        if(abs(uw) > 4)
        {
            ux = 0;
        }

        sprintf(strbuf, "ux %d", ux);
        tft.drawString(strbuf, 40, 110);
        sprintf(strbuf, "uw %d", uw);
        tft.drawString(strbuf, 40, 135);

        Setspeed(ux, BASE_SPEED, uw);

        tft.pushSprite(0,0);
    }
}