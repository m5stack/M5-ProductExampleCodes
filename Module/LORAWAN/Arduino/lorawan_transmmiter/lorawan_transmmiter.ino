/*
    Master.ino
*/
#include <M5Stack.h>
#include <LoRaWan.h>

#define SerialUSB Serial

unsigned char buffer[128] = {
    0,
};
unsigned char buff[] = {2, 3, 4, 5, 12};
long lastSendTime = 0; // last send time
int interval = 300;    // interval between sends
int send_lastSendTime = 0;
int send_interval = 300; // interval between sends
uint8_t flag_test_868 = 0;

void key_scan(void *arg)
{
    while (1)
    {
    if (M5.BtnA.wasPressed())
    {
        M5.Lcd.setCursor(0, 20);
        M5.Lcd.fillRect(0, 20, 320, 215, BLACK);
    }
    if (M5.BtnB.wasPressed())
    {
        flag_test_868 = 0;
        init_433();
        send_data();
        delay(300);
        M5.Lcd.println("btn B  Test frequency 433");
    }
    if (M5.BtnC.wasPressed())
    {
        flag_test_868 = 1;
        init_868();
        send_data();
        delay(300);
        M5.Lcd.println("btn C  Test frequency 868");
    }
    M5.update();
    }
    vTaskDelete(NULL);
}

void setup(void)
{
    M5.begin();
    SerialUSB.begin(9600);
    lora.init();
    delay(2000); // elay for lorawan power on
    lora.initP2PMode(433, SF12, BW500, 8, 8, 20);
    M5.Lcd.setTextFont(2);
    M5.Lcd.println("transmitter");
    M5.Lcd.setTextColor(BLUE);
    int core = xPortGetCoreID();
    M5.Lcd.println(core);
    xTaskCreatePinnedToCore(key_scan, "key_scan", 2048, NULL, 0, NULL, 0);
}

void loop(void)
{
    //   lora.loraDebug();
    if (millis() - lastSendTime > interval)
    {
        Serial2.print("AT+TEST=RXLRPKT\r\n");
        delay(50);
        lora.loraDebug();
        // onReceive();
        lastSendTime = millis();
        interval = random(300) + interval;
    }
    // if (millis() - send_lastSendTime > send_interval)
    // {
    //     send_data();
    //     send_lastSendTime = millis();
    //     send_interval = random(50) + send_interval;
    // }
    // if(flag_test_868){
    //     flag_test_868 = 0;
    //     init_868();
    //     M5.Lcd.println("btn C  Test frequency 868");
    // }
    // if (M5.BtnA.wasPressed())
    // {
    //     M5.Lcd.setCursor(0, 20);
    //     M5.Lcd.fillRect(0, 20, 320, 215, BLACK);
    // }
    // if (M5.BtnB.wasPressed())
    // {
    //     flag_test_868 = 0;
    //     init_433();
    //     Serial.println("btn B  Test frequency 433");
    // }
    // if (M5.BtnC.wasPressed())
    // {
    //     flag_test_868 = 1;
    //     init_868();
    //     Serial.println("btn C  Test frequency 868");
    // }
    // M5.update();
}

void init_433()
{
    lora.initP2PMode(433, SF12, BW500, 8, 8, 20);
}

void init_868()
{
    lora.initP2PMode(868, SF12, BW500, 8, 8, 20);
}

void send_data()
{
    lora.transferPacketP2PMode("hello world");
    //   Serial2.print("AT+TEST=RXLRPKT\r\n");
}

void onReceive()
{
    short length = 0;
    short rssi = 0;

    memset(buffer, 0, 128);
    length = lora.receivePacketP2PMode(buffer, 128, &rssi, 1);

    if (length)
    {
        SerialUSB.print("Length is: ");
        SerialUSB.println(length);
        SerialUSB.print("RSSI is: ");
        SerialUSB.println(rssi);
        SerialUSB.print("Data is: ");
        if (flag_test_868)
        {
            M5.Lcd.print("Test Freq 868:  ");
        }
        else
        {
            M5.Lcd.print("Test Freq 433:  ");
        }
        for (unsigned char i = 0; i < length; i++)
        {
            SerialUSB.print((char)buffer[i]);
            M5.Lcd.print((char)buffer[i]);
            //   SerialUSB.print("0x");
            //   SerialUSB.print(buffer[i], HEX);
            //   SerialUSB.print(" ");
        }
        SerialUSB.println();
        M5.Lcd.println();
    }
}
