
#include <M5Stack.h>               

#define RX_PIN     16
#define TX_PIN    17
#define RESET_PIN   5   // pin to the reset pin Sim800L

#define BUFFER_RESERVE_MEMORY  255
#define DEFAULT_BAUD_RATE   9600
#define TIME_OUT_READ_SERIAL  5000


char* text;
char* number;
bool error; 					//to catch the response of sendSms
String _buffer;

void Reset()
{
    digitalWrite(RESET_PIN,1);
    delay(1000);
    digitalWrite(RESET_PIN,0);
    delay(1000);  
}

void sendsms(char* number,char* text)
{

    Serial2.print (F("AT+CMGF=1\r"));   //set sms to text mode
    Serial2.print (F("AT+CMGS=\""));   // command to send sms
    Serial2.print (number);
    Serial2.print(F("\"\r"));
    Serial2.print (text);
    Serial2.print ("\r");
    Serial2.print((char)26);
    
}

void setup(){
	Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);		
  Serial2.print (F("AT+CMGF=1\r"));   //set sms to text mode
  Serial2.print (F("AT+CMGS=\""));   // command to send sms
  
	text="Testing Sms";  		//text for the message. 
	number="16625113900"; 		//change to a valid number.
	sendsms(number,text);

}

void loop(){
	//do nothing
}
