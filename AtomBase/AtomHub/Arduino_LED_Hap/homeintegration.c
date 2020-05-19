
#include "port_x.h"
#include "xrtos.h"
#include "debug.h"



#include "homeintegration.h"

#include <stdio.h>
#if defined(ESP32) || defined(ESP_IDF)
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <driver/gpio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#endif

#ifdef ESP8266
#include <Arduino.h>
//#include <gpio.h>
#include "port.h"

#endif




#include "storage.h"
#include "storage_ex.h"

#define MAX_SERVICES 20


static callback_storagechanged callbackstorage=NULL;
static int led_gpio = -1;



void hap_set_identity_gpio(int gpio) {
	led_gpio = gpio;
	if (led_gpio > 0) {
#if defined(ESP32)
		gpio_set_direction(led_gpio, GPIO_MODE_OUTPUT);
#elif defined(ESP8266)
		//gpio_enable(led_gpio, OUTPUT);
		pinMode(led_gpio, OUTPUT);
#endif
		led_write(false);
	}

}
void relay_write(int relay, bool on) {
  //  INFO("Relay %d %s\n", relay, on ? "ON" : "OFF");
    gpio_set_level(relay, on ? 1 : 0);
}

void led_write(bool on) {
#if defined(ESP32)
    gpio_set_level(led_gpio, on ? 0 : 1);
#else
	digitalWrite(led_gpio, on ? HIGH : LOW);
#endif
}


#if defined(ESP32)
void identify_task(void *_args) {
	if (led_gpio > 0) {
		led_write( true);

		for (int i = 0; i < 3; i++) {
			led_write(true);
			vTaskDelay(500 / portTICK_PERIOD_MS);
			led_write(false);
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}

		led_write(false);
	}
    vTaskDelete(NULL);
}
#else
LOCAL os_timer_t identtimer;
void identify_task(void *_args) {
	if (led_gpio > 0) {
		led_write(true);

		for (int i = 0; i < 3; i++) {
			led_write(true);
			delay(500);
			led_write(false);
			delay(500);
		}

		led_write(false);
	}
	os_timer_disarm(&identtimer);
}
#endif

void identify(homekit_value_t _value) {
#if defined(ESP32)

   // INFO("LED identify\n");
   // xTaskCreate(identify_task, "LED identify", 2048, NULL, 2, NULL);
#else
	if (led_gpio > 0) {
		os_timer_disarm(&identtimer);
		os_timer_setfn(&identtimer, (os_timer_func_t *)identify_task, (void *)NULL);
		os_timer_arm(&identtimer, 100, false);
	}
#endif
}



homekit_accessory_t *accessories[2];


homekit_server_config_t config = {
    .accessories = accessories,
    .password = "111-11-111"
};

void on_storage_changed(){

	if(callbackstorage)
		callbackstorage(get_ex_storage(),get_ex_storage_size());
}
#ifndef ARDUINO8266_SERVER_CPP
void init_homekit_server() {
	set_callback_storage(on_storage_changed);
    homekit_server_init(&config);
}
#endif
/*
void on_wifi_ready() {
    homekit_server_init(&config);
}
*/
//storage handling


void set_callback_storage_change(callback_storagechanged fn){
	callbackstorage=fn;
}


int hap_get_storage_size_ex(){
	return get_ex_storage_size();
}
int hap_init_storage_ex(char* szdata,int size){
	return init_storage_ex(szdata,size);
}


//ESP Home Controller usage
static int hap_mainservices_current=0;
static int hap_mainaccesories_current=0;

#define MAX_HAP_SERVICES 7
#define MAX_HAP_ACCESSORIES 4
homekit_accessory_t *hap_accessories[MAX_HAP_ACCESSORIES+1]={0};
homekit_service_t* hap_services[MAX_HAP_SERVICES+1]={0};
homekit_server_config_t hap_config = {
    .accessories = hap_accessories,
    .password = "111-11-111"
};
static const char* sz_acc_name=NULL;
static const char* sz_acc_manufacturer=NULL;
static const char* sz_acc_serialnumber=NULL;
static const char* sz_acc_models=NULL;
static const char* sz_acc_firmware=NULL;
static int base_acctype=homekit_accessory_category_other;
static int base_accessory_index=-1;
static bool paired = false;

#ifndef ARDUINO8266_SERVER_CPP
void hap_init_homekit_server() {
	if(hap_mainservices_current>1){
		set_callback_storage(on_storage_changed);
		 paired = homekit_is_paired();
		 INFO("homekit_is_paired %d",paired);
		 if(base_accessory_index==-1){
			 hap_init_homekit_base_accessory();
		 }else{
			 homekit_accessory_t*old=hap_accessories[base_accessory_index];
			 hap_accessories[base_accessory_index] =
					 NEW_HOMEKIT_ACCESSORY(
					 				.category=(homekit_accessory_category_t)base_acctype,//  homekit_accessory_category_lightbulb,
					 				.services=hap_services);
			// old->services = 0;  // do not destruct services
			 //homekit_accessory_free(old,false);
			 //TO DO  memory leak
			 //Need release  homekit_accessory_t * old  
		 }


    	homekit_server_init(&hap_config);
	}else{
		INFO("hap_init_homekit_server nothing to init ");
	}

}
bool hap_homekit_is_paired() {
	return homekit_is_paired();
}
void hap_restart_server() {
	homekit_server_restart();
}
#endif
bool hap_setup_final_step() {
	if (hap_mainservices_current > 1) {
		set_callback_storage(on_storage_changed);
		
		//INFO("homekit_is_paired %d", paired);
		if (base_accessory_index == -1) {
			hap_init_homekit_base_accessory();
		}
		else {
			homekit_accessory_t*old = hap_accessories[base_accessory_index];
			hap_accessories[base_accessory_index] =
				NEW_HOMEKIT_ACCESSORY(
					.category = (homekit_accessory_category_t)base_acctype,//  homekit_accessory_category_lightbulb,
					.services = hap_services);
			 old->services = 0;  // do not destruct services
			homekit_accessory_free(old,false);
			 //TO DO  memory leak
			 //Need release  homekit_accessory_t * old  
			return true;
		}

	}
	else {
		INFO("hap_init_homekit_server nothing to init ");
		return false;
	}
}
homekit_server_config_t* hap_get_server_config() {
	return &hap_config;
}
void hap_init_homekit_base_accessory(){
if(base_accessory_index>=0){
//	INFO("base accessory already set ");
	return;
}
hap_accessories[hap_mainaccesories_current] = NEW_HOMEKIT_ACCESSORY(
				.category=(homekit_accessory_category_t)base_acctype,
				.services=hap_services);
base_accessory_index=hap_mainaccesories_current;
hap_mainaccesories_current++;
hap_accessories[hap_mainaccesories_current] = NULL;
 //INFO("base accessory index %d ",base_accessory_index);
}

void hap_setbase_accessorytype(int val){
	base_acctype=val;
}

int hap_initbase_accessory_service(const char* szname_value,const char* szmanufacturer,const char* szserialnumber,const char* szmodels,const char* szfirmware ){
	sz_acc_name=szname_value;
	sz_acc_manufacturer=szmanufacturer;
	sz_acc_serialnumber=szserialnumber;
	sz_acc_models=szmodels;
	sz_acc_firmware=szfirmware;
	hap_services[0]=hap_new_homekit_accessory_service(szname_value,szserialnumber);

	hap_mainservices_current=1;
//	INFO("hap init base accessory service , next %d",hap_mainservices_current);
	return hap_mainservices_current;
}
void hap_set_device_password(char* szpwd) {
	hap_config.password = szpwd;
}
homekit_service_t* hap_new_homekit_accessory_service(const char *szname,const char * szserialnumber){
	return NEW_HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
		        NEW_HOMEKIT_CHARACTERISTIC(NAME, sz_acc_name),
		        NEW_HOMEKIT_CHARACTERISTIC(MANUFACTURER, sz_acc_manufacturer),
		        NEW_HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, szserialnumber),
		        NEW_HOMEKIT_CHARACTERISTIC(MODEL, sz_acc_models),
		        NEW_HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, sz_acc_firmware),
		        NEW_HOMEKIT_CHARACTERISTIC(IDENTIFY, identify),
		        NULL
		    });
}
homekit_service_t* hap_new_lightbulb_service(const char* szname,hap_callback cb,void* context){

	return NEW_HOMEKIT_SERVICE(LIGHTBULB, .primary = true,.characteristics=(homekit_characteristic_t*[]) {
	            NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
	            NEW_HOMEKIT_CHARACTERISTIC(
	                ON, true,
	                .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(
	                		cb, .context=context
	                ),
	            ),
	            NULL
	        });


}
homekit_service_t* hap_new_lightbulb_dim_service(const char* szname,hap_callback cb,void* context){

	return NEW_HOMEKIT_SERVICE(LIGHTBULB, .primary = true,.characteristics=(homekit_characteristic_t*[]) {
	            NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
	            NEW_HOMEKIT_CHARACTERISTIC(
	                ON, true,
	                .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(
	                		cb, .context=context
	                ),
	            ),
				NEW_HOMEKIT_CHARACTERISTIC(
				  BRIGHTNESS, 100,
				  .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(
				  	cb, .context=context
				  	)
				),
	            NULL
	        });


}
homekit_service_t* hap_add_lightbulb_service(const char* szname,hap_callback cb,void* context){

	return hap_add_service(hap_new_lightbulb_service(szname,cb,context));
}

homekit_service_t*  hap_add_lightbulb_service_as_accessory(int acctype,const char* szname,hap_callback cb,void* context){

	homekit_service_t* baseservice=hap_new_homekit_accessory_service(szname,"0");
	homekit_service_t* lbservice= hap_new_lightbulb_service(szname,cb,context);
	homekit_service_t* svc[3];
	svc[0]=baseservice;//hap_new_homekit_accessory_service(szname,"0");
	svc[1]=lbservice;//hap_new_lightbulb_service(szname,cb,context);
	svc[2]=NULL;
	hap_accessories[hap_mainaccesories_current] = NEW_HOMEKIT_ACCESSORY(
			.category=(homekit_accessory_category_t)acctype,
			.services=svc
							);

	hap_mainaccesories_current++;
    hap_accessories[hap_mainaccesories_current] = NULL;
 //   INFO("added light bulb as accessory , next accessory %d",hap_mainaccesories_current);
return lbservice;
}
homekit_service_t* hap_add_rgbstrip_service(const char* szname,hap_callback cb,void* context){

	homekit_service_t*service=NEW_HOMEKIT_SERVICE(LIGHTBULB,.primary = true, .characteristics=(homekit_characteristic_t*[]) {
	            NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
	            NEW_HOMEKIT_CHARACTERISTIC(
	                ON, true,
	                .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(
	                		cb, .context=context
	                	)
					),

					NEW_HOMEKIT_CHARACTERISTIC(
					  BRIGHTNESS, 100,
					  .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(
					  	cb, .context=context
					  	)
					),

					NEW_HOMEKIT_CHARACTERISTIC(
		                HUE, 0,
						  .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(
						  	cb, .context=context
						  )
		             ),
					 NEW_HOMEKIT_CHARACTERISTIC(
			                SATURATION, 0,

							  .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(
							  	cb, .context=context
						  )
			          ),


	            NULL
	        });
	return hap_add_service(service);
}
homekit_service_t* hap_add_relaydim_service(const char* szname,hap_callback cb,void* context){

	homekit_service_t*service=NEW_HOMEKIT_SERVICE(LIGHTBULB, .characteristics=(homekit_characteristic_t*[]) {
	            NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
	            NEW_HOMEKIT_CHARACTERISTIC(
	                ON, true,
	                .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(
	                		cb, .context=context
	                )
					),
					NEW_HOMEKIT_CHARACTERISTIC(
					  BRIGHTNESS, 100,
					  .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(
					  	cb, .context=context

					  )
					),
	            NULL
	        });
	return hap_add_service(service);
}
homekit_service_t*  hap_add_relaydim_service_as_accessory(int acctype,const char* szname,hap_callback cb,void* context){

	homekit_service_t* baseservice=hap_new_homekit_accessory_service(szname,"0");
	homekit_service_t* lbservice= hap_new_lightbulb_dim_service(szname,cb,context);
	homekit_service_t* svc[3];
	svc[0]=baseservice;
	svc[1]=lbservice;
	svc[2]=NULL;
	hap_accessories[hap_mainaccesories_current] = NEW_HOMEKIT_ACCESSORY(
			.category=(homekit_accessory_category_t)acctype,
			.services=svc
							);

	hap_mainaccesories_current++;
    hap_accessories[hap_mainaccesories_current] = NULL;
 //   INFO("added light bulb as accessory , next accessory %d",hap_mainaccesories_current);
return lbservice;
}
homekit_service_t* hap_add_temperature_service(const char* szname){

	homekit_service_t*service=NEW_HOMEKIT_SERVICE(TEMPERATURE_SENSOR, .characteristics=(homekit_characteristic_t*[]) {
	            NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
	            NEW_HOMEKIT_CHARACTERISTIC(CURRENT_TEMPERATURE, 0),
	            NULL
	        });
	return hap_add_service(service);
}
homekit_service_t* hap_add_humidity_service(const char* szname){

	homekit_service_t*service=NEW_HOMEKIT_SERVICE(HUMIDITY_SENSOR, .characteristics=(homekit_characteristic_t*[]) {
	            NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
	            NEW_HOMEKIT_CHARACTERISTIC(CURRENT_RELATIVE_HUMIDITY, 0),
	            NULL
	        });
	return hap_add_service(service);
}
homekit_service_t*  hap_add_temp_hum_as_accessory(int acctype,const char* szname,homekit_service_t** pp_temp,homekit_service_t** pp_hum){

	homekit_service_t* baseservice=hap_new_homekit_accessory_service(szname,"0");
	homekit_service_t* temp=NEW_HOMEKIT_SERVICE(TEMPERATURE_SENSOR, .primary = true,.characteristics=(homekit_characteristic_t*[]) {
        NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
        NEW_HOMEKIT_CHARACTERISTIC(CURRENT_TEMPERATURE, 0),
        NULL
    });
	homekit_service_t* hum= NEW_HOMEKIT_SERVICE(HUMIDITY_SENSOR,.characteristics=(homekit_characteristic_t*[]) {
        NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
        NEW_HOMEKIT_CHARACTERISTIC(CURRENT_RELATIVE_HUMIDITY, 0),
        NULL
    });
	homekit_service_t* svc[4];
	svc[0]=baseservice;
	svc[1]=temp;
	svc[2]=hum;
	svc[3]=NULL;
	hap_accessories[hap_mainaccesories_current] = NEW_HOMEKIT_ACCESSORY(
			.category=(homekit_accessory_category_t)acctype,
			.services=svc
							);

	hap_mainaccesories_current++;
    hap_accessories[hap_mainaccesories_current] = NULL;
    INFO("add_temp_hum as accessory , next accessory %d",hap_mainaccesories_current);
    if(pp_temp)
    	*pp_temp=temp;
    if(pp_hum)
    	*pp_hum=hum;

return temp;

}
homekit_service_t*  hap_add_temp_as_accessory(int acctype, const char* szname) {

	homekit_service_t* baseservice = hap_new_homekit_accessory_service(szname, "0");
	homekit_service_t* temp = NEW_HOMEKIT_SERVICE(TEMPERATURE_SENSOR, .primary = true, .characteristics = (homekit_characteristic_t*[]) {
		NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
			NEW_HOMEKIT_CHARACTERISTIC(CURRENT_TEMPERATURE, 0),
			NULL
	});
	homekit_service_t* svc[3];
	svc[0] = baseservice;
	svc[1] = temp;
	svc[2] = NULL;

	hap_accessories[hap_mainaccesories_current] = NEW_HOMEKIT_ACCESSORY(
		.category = (homekit_accessory_category_t)acctype,
		.services = svc
	);

	hap_mainaccesories_current++;
	hap_accessories[hap_mainaccesories_current] = NULL;
	INFO("add_temp as accessory , next accessory %d", hap_mainaccesories_current);

	return temp;

}
homekit_service_t*  hap_add_hum_as_accessory(int acctype, const char* szname) {

	homekit_service_t* baseservice = hap_new_homekit_accessory_service(szname, "0");

	homekit_service_t* hum = NEW_HOMEKIT_SERVICE(HUMIDITY_SENSOR, .primary = true, .characteristics = (homekit_characteristic_t*[]) {
		NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
			NEW_HOMEKIT_CHARACTERISTIC(CURRENT_RELATIVE_HUMIDITY, 0),
			NULL
	});
	homekit_service_t* svc[3];
	svc[0] = baseservice;
	svc[1] = hum;
	svc[2] = NULL;
	hap_accessories[hap_mainaccesories_current] = NEW_HOMEKIT_ACCESSORY(
		.category = (homekit_accessory_category_t)acctype,
		.services = svc
	);

	hap_mainaccesories_current++;
	hap_accessories[hap_mainaccesories_current] = NULL;
	INFO("add_hum as accessory , next accessory %d", hap_mainaccesories_current);


	return hum;

}
homekit_service_t* hap_new_light_service(const char* szname, hap_callback cb, void* context){
	return NEW_HOMEKIT_SERVICE(LIGHT_SENSOR, .characteristics=(homekit_characteristic_t*[]) {
		            NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
		            NEW_HOMEKIT_CHARACTERISTIC(CURRENT_AMBIENT_LIGHT_LEVEL, 5.0, .min_value = (float[]) { 0.0 }, .max_value = (float[]) { 100.0 }),
		            NULL
		        });
}
//float min = 0.0;
homekit_service_t* hap_add_light_service(const char* szname, hap_callback cb, void* context){

	homekit_service_t* lightsvc = hap_new_light_service(szname, cb, context);
		/*
		NEW_HOMEKIT_SERVICE(LIGHT_SENSOR, .characteristics = (homekit_characteristic_t*[]) {
		NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
			NEW_HOMEKIT_CHARACTERISTIC(CURRENT_AMBIENT_LIGHT_LEVEL, 5.0,.min_value= (float[]) { 0.0 }, .max_value = (float[]) {100.0 }),
			NULL
	});
	*/
	//lightsvc->characteristics[1]->min_value = &min;
	DEBUG("hap_services added char 1 %f ", lightsvc->characteristics[1]->min_value[0]);
	return hap_add_service(lightsvc);
	//(hap_new_light_service(szname,cb,context));
}

homekit_service_t*  hap_add_light_service_as_accessory(int acctype, const char* szname, hap_callback cb, void* context) {

	homekit_service_t* baseservice = hap_new_homekit_accessory_service(szname, "0");
	homekit_service_t* lightservice = hap_new_light_service(szname, cb, context);
	
	homekit_service_t* svc[3];
	svc[0] = baseservice;
	svc[1] = lightservice;
	svc[2] = NULL;
	hap_accessories[hap_mainaccesories_current] = NEW_HOMEKIT_ACCESSORY(
		.category = (homekit_accessory_category_t)acctype,
		.services = svc
	);

	hap_mainaccesories_current++;
	hap_accessories[hap_mainaccesories_current] = NULL;
	
	return lightservice;
}
homekit_service_t* hap_new_battery_service(const char* szname, hap_callback cb, void* context) {
	return NEW_HOMEKIT_SERVICE(BATTERY_SERVICE, .characteristics = (homekit_characteristic_t*[]) {
			NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
			NEW_HOMEKIT_CHARACTERISTIC(BATTERY_LEVEL, 0),
			NEW_HOMEKIT_CHARACTERISTIC(CHARGING_STATE, 0),
			NEW_HOMEKIT_CHARACTERISTIC(STATUS_LOW_BATTERY, 0),
			NULL
	});
}
homekit_service_t* hap_add_battery_service(const char* szname, hap_callback cb, void* context) {

	INFO("hap_add_battery_service");
	return hap_add_service(hap_new_battery_service(szname, cb, context));
}

homekit_service_t* hap_new_motion_service(const char* szname, hap_callback cb, void* context) {
	return NEW_HOMEKIT_SERVICE(MOTION_SENSOR, .characteristics = (homekit_characteristic_t*[]) {
		NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
		NEW_HOMEKIT_CHARACTERISTIC(MOTION_DETECTED, 0),
		NULL
	});
}
homekit_service_t* hap_add_motion_service(const char* szname, hap_callback cb, void* context) {

	INFO("hap_add_motion_service");
	return hap_add_service(hap_new_motion_service(szname, cb, context));
}
homekit_service_t*  hap_add_motion_service_as_accessory(int acctype, const char* szname, hap_callback cb, void* context) {

	INFO("add hap_add_motion_service as accessory");
	homekit_service_t* baseservice = hap_new_homekit_accessory_service(szname, "0");
	homekit_service_t* motionservice = hap_new_motion_service(szname, cb, context);

	homekit_service_t* svc[3];
	svc[0] = baseservice;
	svc[1] = motionservice;
	svc[2] = NULL;
	hap_accessories[hap_mainaccesories_current] = NEW_HOMEKIT_ACCESSORY(
		.category = (homekit_accessory_category_t)acctype,
		.services = svc
	);

	hap_mainaccesories_current++;
	hap_accessories[hap_mainaccesories_current] = NULL;

	return motionservice;
}
homekit_service_t* hap_new_fan_service(const char* szname, hap_callback cb, void* context) {
	return NEW_HOMEKIT_SERVICE(FAN2, .characteristics = (homekit_characteristic_t*[]) {
		NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
		NEW_HOMEKIT_CHARACTERISTIC(ROTATION_SPEED, 0),
		NULL
	});
}

homekit_service_t* hap_add_fan_service(const char* szname, hap_callback cb, void* context) {

	INFO("hap_add_fan_service");
	return hap_add_service(hap_new_fan_service(szname, cb, context));
}

homekit_service_t* hap_new_switch_service(const char* szname,hap_callback cb,void* context){

	return NEW_HOMEKIT_SERVICE(SWITCH, .primary = true,.characteristics=(homekit_characteristic_t*[]) {
	            NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
	            NEW_HOMEKIT_CHARACTERISTIC(
	                ON, true,
	                .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(
	                		cb, .context=context
	                ),
	            ),
	            NULL
	        });
}

homekit_service_t* hap_add_switch_service(const char* szname,hap_callback cb,void* context){

	return hap_add_service(hap_new_switch_service(szname,cb,context));
}


homekit_service_t* hap_new_button_service(const char* szname/*, hap_callback cb, void* context*/){

	return NEW_HOMEKIT_SERVICE(STATELESS_PROGRAMMABLE_SWITCH,.characteristics=(homekit_characteristic_t*[]) {
	            NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
	            NEW_HOMEKIT_CHARACTERISTIC(PROGRAMMABLE_SWITCH_EVENT, 0,
					//.callback = HOMEKIT_CHARACTERISTIC_CALLBACK(cb, .context = context)
					),
	            NULL
	        });
}


homekit_service_t* hap_add_button_service(const char* szname/*, hap_callback cb, void* context*/) {

	return hap_add_service(hap_new_button_service(szname/*, cb, context*/));
}

homekit_service_t* hap_new_air_quality_service(const char* szname/*, hap_callback cb, void* context*/) {

	return NEW_HOMEKIT_SERVICE(AIR_QUALITY_SENSOR, .characteristics = (homekit_characteristic_t*[]) {
		NEW_HOMEKIT_CHARACTERISTIC(NAME, szname),
			NEW_HOMEKIT_CHARACTERISTIC(AIR_QUALITY, 0,
				//.callback = HOMEKIT_CHARACTERISTIC_CALLBACK(cb, .context = context)
				),
			NEW_HOMEKIT_CHARACTERISTIC(CARBON_DIOXIDE_LEVEL, 0,
				//.callback = HOMEKIT_CHARACTERISTIC_CALLBACK(cb, .context = context)
				),
			NULL
	});
}


homekit_service_t* hap_add_air_quality_service(const char* szname/*, hap_callback cb, void* context*/) {

	return hap_add_service(hap_new_air_quality_service(szname/*, cb, context*/));
}
homekit_service_t*  hap_add_air_quality_service_as_accessory(int acctype, const char* szname/*, hap_callback cb, void* context*/) {

	INFO("add hap_add_air_quality_service as accessory");
	homekit_service_t* baseservice = hap_new_homekit_accessory_service(szname, "0");
	homekit_service_t* airservice = hap_new_air_quality_service(szname/*, cb, context*/);

	homekit_service_t* svc[3];
	svc[0] = baseservice;
	svc[1] = airservice;
	svc[2] = NULL;
	hap_accessories[hap_mainaccesories_current] = NEW_HOMEKIT_ACCESSORY(
		.category = (homekit_accessory_category_t)acctype,
		.services = svc
	);

	hap_mainaccesories_current++;
	hap_accessories[hap_mainaccesories_current] = NULL;

	return airservice;
}

homekit_service_t* hap_add_service(homekit_service_t* service ){
	if(hap_mainservices_current>=MAX_HAP_SERVICES){
		DEBUG("hap_add_service NOT possible, maximum services reached");
		return NULL;
	}
	hap_init_homekit_base_accessory();
	int added = hap_mainservices_current;
	hap_services[hap_mainservices_current]=service;

	hap_mainservices_current++;
	hap_services[hap_mainservices_current]=NULL;
	//INFO("hap_add_service next %d",hap_mainservices_current);
	//INFO("hap_services added %d ", (int)hap_services[added]);
	int i = 0;
	homekit_characteristic_t* it = hap_services[added]->characteristics[i];
	while (it) {
		INFO("hap_services added chararacteristic  %d: %s ", i, it->type);
		i++;
		it = hap_services[added]->characteristics[i];
	}
	//INFO("hap_services added char 0 %d ", (int)hap_services[added]->characteristics[0]);
	//INFO("hap_services added char 1 %d ", (int)hap_services[added]->characteristics[1]);
	//INFO("hap_services added char 2 %d ", (int)hap_services[added]->characteristics[2]);

	return service;
}


