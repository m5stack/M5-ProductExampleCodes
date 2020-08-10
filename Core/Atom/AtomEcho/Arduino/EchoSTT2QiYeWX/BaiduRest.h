#ifndef _BAIDUREST_H_
#define _BAIDUREST_H_

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <driver/i2s.h>


#define  TOKEN_GET_URL      "flow.m5stack.com:5003/token_get"
#define  REST_URL           "flow.m5stack.com:5003/pcm"
#define  REST_URL_T2A       "flow.m5stack.com:5003/text2audio"

#define  JSON_DOCSIZE   512

#define  DEV_PID_MANDARIN   "1537"      //普通话(纯中文识别)
#define  DEV_PID_M_AND_E    "1536"      //普通话(支持简单的英文识别)
#define  DEV_PID_ENGLISH    "1737"      //英语
#define  DEV_PID_CANTONESE  "1637"      //粤语


struct pcmPack
{
    uint8_t *packData;
    uint32_t packsize;
};

class BaiduRest
{
private:
    String  api_token_str;
    StaticJsonDocument<JSON_DOCSIZE>    rest_json_doc;
    char ascii2hex_buff[17] ="0123456789abcdef";

public:
    QueueHandle_t xQ_i2sSteam = NULL;

public:
    BaiduRest();
    ~BaiduRest();
    int gettoken(void);
    int creattoken(void);
    void settoken(String token_str);
    
    int Pcm2String(uint8_t* pcm_buff, uint32_t pcm_lan, String dev_pid, String *results_str);
    int String2Pcm(String str, int spd, int pit, int vol, int per,uint8_t* pcm_buff, size_t* len);
    int String2Pcm(String str, int spd, int pit, int vol, int per);
    String Str2UrlEncode(String str);

};



#endif