//insert CAN Unit to PORT-B,Press buttonA or buttonB to Send message

#include <M5Stack.h>
#include "ESP32CAN.h"
#include "CAN_config.h"

#define TX GPIO_NUM_26
#define RX GPIO_NUM_36

CAN_device_t CAN_cfg;               // CAN Config

int i = 0;

void header(const char *string, uint16_t color)
{
    M5.Lcd.fillScreen(color);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.fillRect(0, 0, 320, 30, TFT_BLACK);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.drawString(string, 160, 3, 4); 
}

void setup() {
  M5.begin(true, false, true);
  M5.Power.begin();
  Serial.println("CAN Unit Send");
  header("CAN-Bus Send", BLACK);
  M5.Lcd.setCursor(0, 60, 4);
  
  CAN_cfg.speed = CAN_SPEED_125KBPS;
  CAN_cfg.tx_pin_id = TX;
  CAN_cfg.rx_pin_id = RX;
  // Init CAN Module
  ESP32Can.CANInit();
}

void loop() {
    CAN_frame_t rx_frame;
    

    if(M5.BtnA.wasPressed()){
        rx_frame.FIR.B.FF = CAN_frame_std;
        rx_frame.MsgID = 1;
        rx_frame.FIR.B.DLC = 8;
        rx_frame.data.u8[0] = 'H';
        rx_frame.data.u8[1] = 'e';
        rx_frame.data.u8[2] = 'l';
        rx_frame.data.u8[3] = 'l';
        rx_frame.data.u8[4] = 'o';
        rx_frame.data.u8[5] = 'C';
        rx_frame.data.u8[6] = 'A';
        rx_frame.data.u8[7] = 'N';
            
        ESP32Can.CANWriteFrame(&rx_frame);
        M5.Lcd.println("Send Message");
    }
    if(M5.BtnB.wasPressed()){
        rx_frame.FIR.B.FF = CAN_frame_std;
        rx_frame.MsgID = 1;
        rx_frame.FIR.B.DLC = 8;
        rx_frame.data.u8[0] = 'M';
        rx_frame.data.u8[1] = '5';
        rx_frame.data.u8[2] = 'S';
        rx_frame.data.u8[3] = 'T';
        rx_frame.data.u8[4] = 'A';
        rx_frame.data.u8[5] = 'C';
        rx_frame.data.u8[6] = 'K';
        rx_frame.data.u8[7] = ' ';
            
        ESP32Can.CANWriteFrame(&rx_frame);
        M5.Lcd.println("Send Message");
        Serial.println("B");
    }
    M5.update();
    delay(200);

}
