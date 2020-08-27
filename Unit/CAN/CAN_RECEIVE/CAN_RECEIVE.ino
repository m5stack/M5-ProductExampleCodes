//Insert CAN Unit to PORT-B,if received message, screen will display

#include <M5Stack.h>
#include "ESP32CAN.h"
#include "CAN_config.h"

#define TX GPIO_NUM_26
#define RX GPIO_NUM_36

CAN_device_t CAN_cfg;

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
    Serial.begin(115200);
    Serial.println("CAN Unit Receive");
    header("CAN-Bus Receive", BLACK);
    
    CAN_cfg.speed=CAN_SPEED_125KBPS;
    CAN_cfg.tx_pin_id = TX;
    CAN_cfg.rx_pin_id = RX;
    CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
    //start CAN Module
    ESP32Can.CANInit();
}

void loop() {
    CAN_frame_t rx_frame;
    //receive next CAN frame from queue
    if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){

      M5.Lcd.fillRect(0, 60, 320, 180, BLACK);
      M5.Lcd.setCursor(0, 60, 4);
      //do stuff!
      if(rx_frame.FIR.B.FF==CAN_frame_std){
        printf("New standard frame");
        M5.Lcd.printf("New standard frame");
      } else{
        printf("New extended frame");
        M5.Lcd.printf("New extended frame");
      }
      if(rx_frame.FIR.B.RTR==CAN_RTR){
        printf(" RTR from 0x%08x, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        M5.Lcd.printf(" RTR from 0x%08x, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      } else{
        printf(" from 0x%08x, DLC %d\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        M5.Lcd.printf(" from 0x%08x, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        for(int i = 0; i < 8; i++){
          printf("%c\t", (char)rx_frame.data.u8[i]);
          M5.Lcd.printf("%c\t", (char)rx_frame.data.u8[i]);
        }
        printf("\n");
      }
    }
}
