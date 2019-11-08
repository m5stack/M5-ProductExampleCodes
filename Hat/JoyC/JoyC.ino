#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiUdp.h>

TFT_eSprite Disbuff = TFT_eSprite(&M5.Lcd);
extern const unsigned char connect_on[800];
extern const unsigned char connect_off[800];

#define SYSNUM 		3

uint64_t realTime[4], time_count = 0;
bool k_ready = false;
uint32_t key_count = 0;

IPAddress local_IP(192, 168, 4, 100 + SYSNUM );
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

const char *ssid = "HadesAP2468";
const char *password = "77777777";

WiFiUDP Udp;  
uint32_t send_count = 0;
uint8_t system_state = 0;


void I2CWrite1Byte( uint8_t Addr ,  uint8_t Data )
{
	Wire.beginTransmission(0x38);
	Wire.write(Addr);
	Wire.write(Data);
	Wire.endTransmission();
}

void I2CWritebuff( uint8_t Addr,  uint8_t* Data, uint16_t Length )
{
	Wire.beginTransmission(0x38);
	Wire.write(Addr);
	for (int i = 0; i < Length; i++)
	{
		Wire.write(Data[i]);
	}
	Wire.endTransmission();
}

uint8_t I2CRead8bit( uint8_t Addr )
{
	Wire.beginTransmission(0x38);
	Wire.write(Addr);
	Wire.endTransmission();
	Wire.requestFrom(0x38, 1);
	return Wire.read();
}

uint16_t I2CRead16bit( uint8_t Addr )
{
	uint16_t ReData = 0;
	Wire.beginTransmission(0x38);
	Wire.write(Addr);
	Wire.endTransmission();
	Wire.requestFrom(0x38, 2);
	for( int i = 0 ; i < 2 ; i++ )
	{
		ReData <<= 8;
		ReData |= Wire.read();
	}
	return ReData;
}

bool joys_l = false ;
uint8_t color[3] = {0,100,0};



uint8_t SendBuff[8] = 	{ 0xAA, 0x55, 
						 SYSNUM,
						 0x00, 0x00,
						 0x00, 0x00,
						 0xee};

void SendUDP()
{
	if( WiFi.status() == WL_CONNECTED )
	{
		Udp.beginPacket(IPAddress(192,168,4,1),1000 + SYSNUM );
		Udp.write(SendBuff,8);
		Udp.endPacket();
	}
}

void setup() {
  // put your setup code here, to run once:
    M5.begin();
    M5.update();
    Wire.begin(0,26,10000);


	M5.Lcd.setRotation(4);
    M5.Lcd.setSwapBytes(false);
    Disbuff.createSprite(80, 160);
    Disbuff.setSwapBytes(true);

	Disbuff.fillRect(0,0,80,20,Disbuff.color565(50,50,50));
	Disbuff.setTextSize(1);
	Disbuff.setTextColor(WHITE);
	Disbuff.setCursor(55,6);
	Disbuff.printf("%03d",SYSNUM);

	Disbuff.pushImage(0,0,20,20,(uint16_t *)connect_off);
	Disbuff.pushSprite(0,0);

	//I2CWrite1Byte(0x10,0x02);

	/*
	
	if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) 
	{
    	Serial.println("STA Failed to configure");
  	}

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
  	}

	Udp.begin(2000);
	*/
	
	Disbuff.pushImage(0,0,20,20,(uint16_t *)connect_on);
	Disbuff.pushSprite(0,0);
}


uint8_t adc_value[5] = {0};
uint16_t AngleBuff[4];
uint32_t count = 0;
void loop() {

	for (int i = 0; i < 5; i++)
	{
		adc_value[i] =  I2CRead8bit( 0x60 + i );
	}

	for (int i = 0; i < 4; i++)
	{
		AngleBuff[i] = 	I2CRead16bit( 0x50 + i * 2 );
	}

	delay(10);
	/*
	
	if( WiFi.status() != WL_CONNECTED )
	{
		Disbuff.pushImage(0,0,20,20,(uint16_t *)connect_off);
		Disbuff.pushSprite(0,0);

		count ++;
		if( count > 500 )
		{
			WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
			count = 0;
		}
	}
	else
	{
		Disbuff.pushImage(0,0,20,20,(uint16_t *)connect_on);
		Disbuff.pushSprite(0,0);
		count = 0;
		if(( AngleBuff[2] != 0 )||( AngleBuff[3] != 0 ))
		{
			SendBuff[3] = adc_value[0];
			SendBuff[4] = adc_value[1];
			SendBuff[5] = adc_value[2];
			SendBuff[6] = 0x01;
		}
		else
		{
			SendBuff[6] = 0x00;
		}
		SendUDP();
	}
	*/

	Disbuff.fillRect( 0,30,80,130,BLACK);
	Disbuff.setCursor(10,30);
	Disbuff.printf("%04d",AngleBuff[0]);
	Disbuff.setCursor(10,40);
	Disbuff.printf("%04d",AngleBuff[1]);
	Disbuff.setCursor(10,50);
	Disbuff.printf("%04d",AngleBuff[2]);
	Disbuff.setCursor(10,60);
	Disbuff.printf("%04d",AngleBuff[3]);
	Disbuff.setCursor(10,70);
	Disbuff.printf("%04X",adc_value[4]);
	Disbuff.pushSprite(0,0);
}