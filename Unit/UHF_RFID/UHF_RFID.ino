#include <M5Stack.h>
#include "RFID_command.h"

UBYTE RFID_debug = 0;

String DATA_Str_Serial = "";
String DATA_Str_M5led = "";
UBYTE DATA_I[4096] = {};  //Used to store distance data
UWORD DATA_I_NUB = 0;
UBYTE DATA_Interim_order[40] = {};
UBYTE DATA_Interim_b = 0;
UBYTE FLAH_S = 0;


int comd = 0;

void setup()
{
  M5.begin();

  Serial2.begin(115200, SERIAL_8N1, 16, 17);//16.17
  
  RFID_Delay(100);

  RFID_Readcallback();
  RFID_clean_data();
  M5.Lcd.fillRect(0, 0, 340, 280, BLACK);

  RFID_Set_transmission_Power(2600);

}

void loop()
{
    M5.Lcd.fillCircle(310, 10, 6, GREEN);
    RFID_Delay(150);
    M5.Lcd.fillCircle(310, 10, 6, BLACK);
    RFID_Delay(150);

    comd = RFID_A_single_poll_of_instructions();
    RFID_Delay(2000);
    RFID_clean_data();
    M5.Lcd.fillRect(0, 0, 340, 280, BLACK);
    
//    RFID_Set_the_Select_mode(0x01);
//    RFID_Delay(2000);
//    RFID_clean_data();
//    M5.Lcd.fillRect(0, 0, 340, 280, BLACK);
//    
//    RFID_The_label_store_writes_data(0x0000ffff)//或者0x00000000
//    RFID_Delay(2000);
//    RFID_clean_data();
//    M5.Lcd.fillRect(0, 0, 340, 280, BLACK);

}
