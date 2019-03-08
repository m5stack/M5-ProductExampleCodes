#include <M5Stack.h>
#include "finger.h"

//Fingerprint module
 
uint8_t TxBuf[9];
uint8_t RxBuf[9];
uint8_t RxCnt;

uint8_t fpm_sendAndReceive(uint16_t timeout)
{
    uint8_t  i, j;
    uint8_t checkSum = 0;
	 
    RxCnt = 0;
	TxBuf[5] = 0;
  
    Serial.write(CMD_HEAD);		
    for (i = 1; i < 6; i++)       
    {
        Serial.write(TxBuf[i]);		 
        checkSum ^= TxBuf[i];
    }
    Serial.write(checkSum);    
    Serial.write(CMD_TAIL);    
    
    while (RxCnt < 8 && timeout > 0)
    {
        delay(1);
        timeout--;		   
    }
    
    uint8_t ch;
    for(i=0;i<8;i++)
    {
        if(Serial.available()){
            ch = Serial.read();
            RxCnt++;
            RxBuf[i] = ch;
        }
      }

    if (RxCnt != 8) {RxCnt = 0;return ACK_TIMEOUT;}
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

uint8_t fpm_sleep(void)
{
    uint8_t res;
    
    TxBuf[CMD] = CMD_SLEEP_MODE;
    TxBuf[P1] = 0;
    TxBuf[P2] = 0;
    TxBuf[P3] = 0;
    
    res = fpm_sendAndReceive(500);
    
    if(res == ACK_SUCCESS) {
        return ACK_SUCCESS;
    }
    else {
        return ACK_FAIL;
    }
  
}

uint8_t fpm_setAddMode(uint8_t fpm_mode)
{
    uint8_t res;
    
    TxBuf[CMD] = CMD_ADD_MODE;
    TxBuf[P1] = 0;
    TxBuf[P2] = fpm_mode;
    TxBuf[P3] = 0;
    
    res = fpm_sendAndReceive(200);
    
    if(res == ACK_SUCCESS && RxBuf[Q3] == ACK_SUCCESS) {
        return ACK_SUCCESS;
    }
    else {
        return ACK_FAIL;
    }
}

uint8_t fpm_readAddMode(void)
{ 
    TxBuf[CMD] = CMD_ADD_MODE;
    TxBuf[P1] = 0;
    TxBuf[P2] = 0;
    TxBuf[P3] = 0X01;
    
    fpm_sendAndReceive(200);
    
    return RxBuf[Q2];
}

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

uint8_t fpm_deleteUser(uint8_t userNum)
{
    uint8_t res;
    
    TxBuf[CMD] = CMD_DEL;
    TxBuf[P1] = 0;
    TxBuf[P2] = userNum;
    TxBuf[P3] = 0;
    
    res = fpm_sendAndReceive(200);
    
    if(res == ACK_SUCCESS && RxBuf[Q3] == ACK_SUCCESS) {
        return ACK_SUCCESS;
    }
    else {
        return ACK_FAIL;
    }
}

uint8_t fpm_addUser(uint8_t userNum, uint8_t userPermission)
{
    uint8_t res;
    
    TxBuf[CMD] = CMD_ADD_1;
    TxBuf[P1] = 0;
    TxBuf[P2] = userNum;
    TxBuf[P3] = userPermission;
    
    res = fpm_sendAndReceive(30000);
    
    if(res == ACK_SUCCESS) {
        if(RxBuf[Q3] == ACK_SUCCESS) {
        TxBuf[CMD] = CMD_ADD_2;
        
        res = fpm_sendAndReceive(30000);
        
        if(res == ACK_SUCCESS) {
            if(RxBuf[Q3] == ACK_SUCCESS) {
            TxBuf[CMD] = CMD_ADD_3;
            
            res = fpm_sendAndReceive(30000);
            
            if(res == ACK_SUCCESS) {
                return RxBuf[Q3];
            }  
            }
        }
        }
    }
    return res;
 
}

uint8_t fpm_compareFinger(void)
{
    uint8_t res;
    
    TxBuf[CMD] = CMD_MATCH;
    TxBuf[P1] = 0;
    TxBuf[P2] = 0;
    TxBuf[P3] = 0;
    
    res = fpm_sendAndReceive(30000);
    
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
 
 
