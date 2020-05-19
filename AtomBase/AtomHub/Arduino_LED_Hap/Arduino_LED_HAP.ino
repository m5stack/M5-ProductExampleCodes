/*The control pin of the relay is GPIO21, which is powered by an external power supply. 
  It is connected to Apple's Homekit application through WiFi for remote control. 
  Open the Homekit for manual connection. The default password is 11111111. 
  Use the Siri voice remote control or manual button for remote control.

  NOTE: YOU SHOULD UNZIP wolfSSL_3_13_0.rar to Arduino Library
*/


#include <M5Atom.h>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>

const char* ssid     = "M5-2.4G";
const char* password = "Office@888888";

const int identity_led=-1;
const int led_gpio = 21;
int flag = 0;


extern "C"{
#include "homeintegration.h"
}
homekit_service_t* hapservice={0};
String pair_file_name="/pair.dat";

void setup() {
    M5.begin(true, false, true);
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network
     if (!SPIFFS.begin(true)) {
       Serial.print("SPIFFS Mount failed");
     }
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        M5.dis.drawpix(0, 0xf0f0f0);
        M5.update();
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    pinMode(led_gpio,OUTPUT);
///setup identity gpio
    hap_set_identity_gpio(identity_led);    //identity_led=2 will blink on identity

/// now will setup homekit device

    //this is for custom storaage usage
    // In given example we are using \pair.dat   file in our spiffs system
    //see implementation below
    init_hap_storage();
    set_callback_storage_change(storage_changed);

    /// We will use for this example only one accessory (possible to use a several on the same esp)
    //Our accessory type is light bulb , apple interface will proper show that
    hap_setbase_accessorytype(homekit_accessory_category_lightbulb);
    /// init base properties
    hap_initbase_accessory_service("LED","M5Stack","2020.5","EspHapLed","1.0.1");

   //we will add only one light bulb service and keep pointer for nest using
    hapservice= hap_add_lightbulb_service("Led",led_callback,(void*)&led_gpio);

   //and finally init HAP
    hap_init_homekit_server();
}

void loop() {
if(M5.Btn.wasPressed()){
  if(!flag){
    digitalWrite(led_gpio, LOW);
    set_led(1);
  }else{
    set_led(0);
    digitalWrite(led_gpio, HIGH);
  }
}
delay(10);
M5.update();
}

void init_hap_storage(){
  Serial.print("init_hap_storage");
 
  
  File fsDAT=SPIFFS.open(pair_file_name, "r");
  if(!fsDAT){
    Serial.println("Failed to read pair.dat");
    return;
  }
  int size=hap_get_storage_size_ex();
  char* buf=new char[size];
  memset(buf,0xff,size);
  int readed=fsDAT.readBytes(buf,size);
  Serial.print("Readed bytes ->");
  Serial.println(readed);
  hap_init_storage_ex(buf,size);
  fsDAT.close();
  delete []buf;

}
void storage_changed(char * szstorage,int size){



  SPIFFS.remove(pair_file_name);
  File fsDAT=SPIFFS.open(pair_file_name, "w+");
  if(!fsDAT){
    Serial.println("Failed to open pair.dat");
    return;
  }
  fsDAT.write((uint8_t*)szstorage, size);

  fsDAT.close();
}
//can be used for any logic, it will automatically inform Apple about state changes
void set_led(bool val){
   Serial.println("set_led"); 
   if(val == 1){
      flag = 1;
      digitalWrite(led_gpio, HIGH);
      M5.dis.drawpix(0, 0x00ff00);
   }else{
      flag = 0;
      digitalWrite(led_gpio, LOW);
      M5.dis.drawpix(0, 0xff0000);
   }
   
  
  //we need notify apple about changes
  
  
  if(hapservice){
    Serial.println("notify hap"); 
    //getting on/off characteristic
    homekit_characteristic_t * ch= homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_ON);
    if(ch){
        Serial.println("found characteristic"); 
        if(ch->value.bool_value!=val){  //wil notify only if different
          ch->value.bool_value=val;
          homekit_characteristic_notify(ch,ch->value);
        }
    }
  }
}

void led_callback(homekit_characteristic_t *ch, homekit_value_t value, void *context) {
    Serial.println("led_callback");
 set_led(ch->value.bool_value);
}
