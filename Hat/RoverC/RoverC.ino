#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiUdp.h>

const char *ssid = "HadesAP";
const char *password = "77777777";

TFT_eSprite Disbuff = TFT_eSprite(&M5.Lcd);
WiFiServer server(80);

WiFiUDP Udp1;

void setup()
{
    M5.begin();
    M5.update();
	Wire.begin(0,26,10000);

	uint64_t chipid=ESP.getEfuseMac();
	String str = ssid + String((uint32_t)(chipid>>32),HEX);
	M5.Lcd.setRotation(4);
	M5.Lcd.setSwapBytes(false);
	Disbuff.createSprite(80, 160);
	Disbuff.setSwapBytes(true);
	Disbuff.fillRect(0, 0, 80, 20, Disbuff.color565(50, 50, 50));
	Disbuff.setTextSize(1);
	Disbuff.setTextColor(WHITE);
	Disbuff.setCursor(0, 6);
	Disbuff.print(str);
	Disbuff.pushSprite(0, 0);

	Serial.begin(115200);
	//Set device in STA mode to begin with
	WiFi.softAPConfig(IPAddress(192, 168, 4, 1),
					  IPAddress(192, 168, 4, 1),
					  IPAddress(255, 255, 255, 0));
	
	WiFi.softAP(str.c_str(), password);
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.begin();

	Udp1.begin(1003);
}

uint8_t SendBuff[9] = {0xAA, 0x55,
					   0x00,
					   0x00,
					   0x00,
					   0x00,
					   0x00,
					   0x00,
					   0xee};


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

int16_t speed_buff[4] = {0};
int8_t  speed_sendbuff[4] = {0};
uint32_t count = 0;

void Setspeed( int16_t Vtx, int16_t Vty, int16_t Wt)
{
	Wt = ( Wt > 100 )  ? 100 :  Wt;
	Wt = ( Wt < -100 ) ? -100 : Wt;

	Vtx = ( Vtx > 100 )  ? 100 :  Vtx;
	Vtx = ( Vtx < -100 ) ? -100 : Vtx;
	Vty = ( Vty > 100 )  ? 100 :  Vty;
	Vty = ( Vty < -100 ) ? -100 : Vty;

	Vtx = ( Wt != 0 ) ? Vtx *  (100- abs(Wt)) / 100 : Vtx;
	Vty = ( Wt != 0 ) ? Vty *  (100- abs(Wt)) / 100 : Vty;

	speed_buff[0] = Vty - Vtx - Wt ;
	speed_buff[1] = Vty + Vtx + Wt ;
	speed_buff[3] = Vty - Vtx + Wt ;
	speed_buff[2] = Vty + Vtx - Wt ;

	for (int i = 0; i < 4; i++)
	{
		speed_buff[i] = ( speed_buff[i] > 100 )  ? 100 :  speed_buff[i];
		speed_buff[i] = ( speed_buff[i] < -100 ) ? -100 : speed_buff[i];
		speed_sendbuff[i] = speed_buff[i];
	}
}

void loop()
{
	int udplength = Udp1.parsePacket();
	if( udplength )
	{
		char udodata[udplength];
		Udp1.read( udodata, udplength);
		IPAddress udp_client = Udp1.remoteIP();
		if(( udodata[0] == 0xAA )&&( udodata[1] == 0x55 )&&( udodata[7] == 0xee ))
		{
			for (int i = 0; i < 8; i++)
			{
				Serial.printf("%02X ",udodata[i]);
			}
			Serial.println();
			if( udodata[6] == 0x01 )
			{
				Setspeed( udodata[3] - 100  ,udodata[4] - 100 , udodata[5] -100 );
			}
			else
			{
				Setspeed( 0  ,0 , 0 );
			}
			
		}
	}
	count ++;   
}
