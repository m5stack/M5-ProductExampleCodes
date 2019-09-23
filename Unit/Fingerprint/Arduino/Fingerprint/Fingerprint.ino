#include <M5Stack.h>

//Fingerprint module

//Response Information
#define ACK_SUCCESS    0x00
#define ACK_FAIL       0x01
#define ACK_FULL       0x04
#define ACK_NOUSER     0x05
#define ACK_USER_EXIST 0x07
#define ACK_TIMEOUT    0x08

//Index Definition
#define HEAD 0
#define CMD  1
#define CHK  6
#define TAIL 7

#define P1   2
#define P2   3
#define P3   4
#define Q1   2
#define Q2   3
#define Q3   4

//Command
#define CMD_HEAD          0xF5
#define CMD_TAIL          0xF5
#define CMD_ADD_1         0x01
#define CMD_ADD_2         0x02
#define CMD_ADD_3         0x03
#define CMD_MATCH         0x0C
#define CMD_DEL           0x04
#define CMD_DEL_ALL       0x05
#define CMD_USER_CNT      0x09
#define CMD_SLEEP_MODE    0x2C
#define CMD_ADD_MODE      0x2D

#define CMD_FINGER_DETECTED 0x14

uint8_t TxBuf[9];        //Send Packets
uint8_t RxBuf[9];        //Receive Packets
uint8_t RxCnt;

/**
  * @brief  Send commands to fingerprint module through serial port and receive response commands
  * @param  timeout：Timeout time (ms), if no response instruction is received within this time, the timeout is processed.
  * @retval Response message
  */
uint8_t fpm_sendAndReceive(uint16_t timeout)
{
    uint8_t  i, j;
    uint8_t checkSum = 0;

    RxCnt = 0;
    TxBuf[5] = 0; //The sixth bit of command data is always zero

    /*Send 8-bit packets*/
    Serial2.write(CMD_HEAD);     //0
    for (i = 1; i < 6; i++)       //1-5
    {
        Serial2.write(TxBuf[i]);
        checkSum ^= TxBuf[i];
    }
    Serial2.write(checkSum);    //6   Check
    Serial2.write(CMD_TAIL);    //7

    while (RxCnt < 8 && timeout > 0)
    {
        delay(1);
        timeout--;
    }

    uint8_t ch;
    for(i=0;i<8;i++)
    {
        if(Serial2.available()){
            ch = Serial2.read();
            RxCnt++;
            RxBuf[i] = ch;
        }
      }

    if (RxCnt != 8) {RxCnt=0;return ACK_TIMEOUT;}
    if (RxBuf[HEAD] != CMD_HEAD) return ACK_FAIL;
    if (RxBuf[TAIL] != CMD_TAIL) return ACK_FAIL;
    if (RxBuf[CMD] != (TxBuf[CMD])) return ACK_FAIL;

    checkSum = 0;
    for (j = 1; j < CHK; j++) {
        checkSum ^= RxBuf[j];
    }
        if (checkSum != RxBuf[CHK]) {
        return ACK_FAIL;
    }
        return ACK_SUCCESS;
}

/**
  * @brief  Get the number of users (actually the number of fingerprints)
  * @param  none
  * @retval Number of fingerprints
  */
uint16_t fpm_getUserNum(void)
{
    uint8_t res;

    TxBuf[CMD] = CMD_USER_CNT;
    TxBuf[P1] = 0;
    TxBuf[P2] = 0;
    TxBuf[P3] = 0;

    res = fpm_sendAndReceive(200);

    if(res == ACK_SUCCESS && RxBuf[Q3] == ACK_SUCCESS) {
        return RxBuf[Q2];
    }
    else {
        return 0XFF;
    }
}

/**
  * @brief  Delete all fingerprints
  * @param  none
  * @retval (ACK_SUCCESS  ACK_FAIL)
  */
uint8_t fpm_deleteAllUser(void)
{
    uint8_t res;

    TxBuf[CMD] = CMD_DEL_ALL;
    TxBuf[P1] = 0;
    TxBuf[P2] = 0;
    TxBuf[P3] = 0;

    res = fpm_sendAndReceive(200);

    if(res == ACK_SUCCESS && RxBuf[Q3] == ACK_SUCCESS) {
        return ACK_SUCCESS;
    }
    else {
        return ACK_FAIL;
    }
}
/**
  * @brief  Typing fingerprint,This process takes about nine seconds.
  * @param  userNum：Location of fingerprints(1-255)
  * @param  userPermission:(1-3),Self-definition of concrete meaning.
  * @retval (ACK_SUCCESS  ACK_FAIL  ACK_USER_EXIST  ACK_TIMEOUT)
  */
uint8_t fpm_addUser(uint8_t userNum, uint8_t userPermission)
{
    uint8_t res;

    TxBuf[CMD] = CMD_ADD_1;
    TxBuf[P1] = 0;
    TxBuf[P2] = userNum;
    TxBuf[P3] = userPermission;

    res = fpm_sendAndReceive(3000);           //First time

    if(res == ACK_SUCCESS) {
        if(RxBuf[Q3] == ACK_SUCCESS) {
        TxBuf[CMD] = CMD_ADD_2;

        res = fpm_sendAndReceive(3000);       //Second time

        if(res == ACK_SUCCESS) {
            if(RxBuf[Q3] == ACK_SUCCESS) {
            TxBuf[CMD] = CMD_ADD_3;

            res = fpm_sendAndReceive(3000);    //Third time

            if(res == ACK_SUCCESS) {
                return RxBuf[Q3];
            }
          }
        }
      }
    }
    return res;
}

/**
  * @brief  Comparing fingerprints by 1:N
  * @param  none
  * @retval Information matching fingerprints
  */
uint8_t fpm_compareFinger(void)
{
    uint8_t res;

    TxBuf[CMD] = CMD_MATCH;
    TxBuf[P1] = 0;
    TxBuf[P2] = 0;
    TxBuf[P3] = 0;

    res = fpm_sendAndReceive(3000);

    if(res == ACK_SUCCESS)
    {
        if(RxBuf[Q3] == ACK_NOUSER) {
        return ACK_NOUSER;
        }
        if(RxBuf[Q3] == ACK_TIMEOUT) {
        return ACK_TIMEOUT;
        }
        if((RxBuf[Q2] != 0) && (RxBuf[Q3] == 1 || RxBuf[Q3] == 2 || RxBuf[Q3] == 3)) {
        return ACK_SUCCESS;
        }
    }
    return res;
}


uint8_t userNum;           //User number
void CleanScreen()
{
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.fillRect(0,50,400,300,BLACK);
    M5.Lcd.setCursor(0, 50);
    M5.Lcd.setTextSize(2);
    userNum = fpm_getUserNum();
    M5.Lcd.print("userNum:");
    M5.Lcd.println(userNum);
}

void setup() {
    M5.begin();
    Serial.begin(19200);
    Serial2.begin(19200, SERIAL_8N1, 16, 17);
    M5.Lcd.clear(BLACK);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextFont(2);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(20, 0);
    M5.Lcd.println("Finger example");
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.fillRect(0,50,400,300,BLACK);
    M5.Lcd.setCursor(0, 50);
    M5.Lcd.setTextSize(2);
    userNum = fpm_getUserNum();
    M5.Lcd.print("userNum:");
    M5.Lcd.println(userNum);
}

//ButtonA: Add user
//ButtonB: Matching
//ButtonC: Delete All User
void loop(){
    uint8_t res1;
    if(M5.BtnA.wasPressed()){
        CleanScreen();
        M5.Lcd.println("Fingerprint Typing");

        res1 = fpm_addUser(userNum,1);
        if(res1 == ACK_SUCCESS){
            M5.Lcd.println("Success");
        }
        else if(res1 == ACK_FAIL){
            M5.Lcd.println("Fail");
        }
        else if(res1 == ACK_FULL){
            M5.Lcd.println("Full");
        }
        else{
            M5.Lcd.println("Timeout");
        }
        userNum++;
    }

    if(M5.BtnB.wasPressed()){
      CleanScreen();
      M5.Lcd.println("Matching");

      res1 = fpm_compareFinger();
      if(res1 == ACK_SUCCESS){
          M5.Lcd.println("Success");
      }
      if(res1 == ACK_NOUSER){
          M5.Lcd.println("No Such User");
      }
      if(res1 == ACK_TIMEOUT){
          M5.Lcd.println("Timeout");
      }
    }

    if(M5.BtnC.wasPressed()){
      res1 = fpm_deleteAllUser();
      CleanScreen();

      if(res1 == ACK_SUCCESS){
          M5.Lcd.println("Delete All User Successful");
      }
      else{
          M5.Lcd.println("Delete All User Failed");
      }
    }
    M5.update();
}
