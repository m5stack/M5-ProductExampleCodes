#pragma once

#include "port_x.h"



#include <stdlib.h>
#include <stdio.h>
#include "homekit.h"
#include "characteristics.h"
#include "storage.h"
#include "storage_ex.h"
#ifdef ARDUINO8266_SERVER_CPP
//#include "arduino_homekit_server.h"
#endif

#define INFO(message, ...) printf(">>> Home Integration: " message "\n", ##__VA_ARGS__)

#ifndef HAP_NOTIFY_CHANGES
#define HAP_NOTIFY_CHANGES(name,home_characteristic,val,tollerance) \
	if (home_characteristic && abs(home_characteristic->value.name ##_value - val)>tollerance){ \
		home_characteristic->value.name ##_value = val ;\
		homekit_characteristic_notify(home_characteristic, home_characteristic->value); \
	};
#endif



	void init_accessory();
	void init_homekit_server();

	typedef void(*callback_storagechanged)(char * szstorage, int size);
	void set_callback_storage_change(callback_storagechanged fn);

	int hap_init_storage_ex(char* szdata, int size);
	int hap_get_storage_size_ex();
	void hap_set_identity_gpio(int gpio);
	void hap_set_device_password(char* szpwd);
	//esp controller usage
	int hap_initbase_accessory_service(const char* szname_value, const char* szmanufacturer, const char* szserialnumber, const char* szmodels, const char* szfirmware);
	homekit_service_t* hap_new_homekit_accessory_service(const char *szname, const char *szserialnumber);
	typedef void(*hap_callback)(homekit_characteristic_t *ch, homekit_value_t value, void *context);
	homekit_service_t* hap_add_lightbulb_service(const char* szname, hap_callback cb, void* context);
	homekit_service_t* hap_new_lightbulb_service(const char* szname, hap_callback cb, void* context);
	homekit_service_t* hap_new_lightbulb_dim_service(const char* szname, hap_callback cb, void* context);
	homekit_service_t*  hap_add_relaydim_service_as_accessory(int acctype, const char* szname, hap_callback cb, void* context);
	homekit_service_t*  hap_add_lightbulb_service_as_accessory(int acctype, const char* szname, hap_callback cb, void* context);

	homekit_service_t* hap_add_rgbstrip_service(const char* szname, hap_callback cb, void* context);
	homekit_service_t* hap_add_relaydim_service(const char* szname, hap_callback cb, void* context);
	homekit_service_t* hap_add_temperature_service(const char* szname);
	homekit_service_t* hap_add_humidity_service(const char* szname);
	homekit_service_t*  hap_add_temp_hum_as_accessory(int acctype, const char* szname, homekit_service_t** pp_temp, homekit_service_t** pp_hum);
	homekit_service_t*  hap_add_hum_as_accessory(int acctype, const char* szname);
	homekit_service_t*  hap_add_temp_as_accessory(int acctype, const char* szname);
	homekit_service_t* hap_add_pressure_service(const char* szname);

	homekit_service_t* hap_add_light_service(const char* szname, hap_callback cb, void* context);
	homekit_service_t* hap_new_light_service(const char* szname, hap_callback cb, void* context);
	homekit_service_t*  hap_add_light_service_as_accessory(int acctype, const char* szname, hap_callback cb, void* context);

	homekit_service_t* hap_add_battery_service(const char* szname, hap_callback cb, void* context);
	homekit_service_t* hap_new_battery_service(const char* szname, hap_callback cb, void* context);
	
	//switch
	homekit_service_t* hap_add_switch_service(const char* szname, hap_callback cb, void* context);
	homekit_service_t* hap_new_switch_service(const char* szname, hap_callback cb, void* context);
	
	//button
	homekit_service_t* hap_add_button_service(const char* szname);// , hap_callback cb, void* context);
	homekit_service_t* hap_new_button_service(const char* szname);// , hap_callback cb, void* context);

	//motion
	homekit_service_t* hap_new_motion_service(const char* szname, hap_callback cb, void* context);
	homekit_service_t* hap_add_motion_service(const char* szname, hap_callback cb, void* context);
	homekit_service_t* hap_add_motion_service_as_accessory(int acctype, const char* szname, hap_callback cb, void* context);
	homekit_service_t* hap_new_fan_service(const char* szname, hap_callback cb, void* context);
	homekit_service_t* hap_add_fan_service(const char* szname, hap_callback cb, void* context);

	//air quality
	homekit_service_t* hap_new_air_quality_service(const char* szname/*, hap_callback cb, void* context*/);
	homekit_service_t* hap_add_air_quality_service(const char* szname/*, hap_callback cb, void* context*/);
	homekit_service_t*  hap_add_air_quality_service_as_accessory(int acctype, const char* szname/*, hap_callback cb, void* context*/);


	homekit_service_t* hap_add_service(homekit_service_t* service);

	void hap_setbase_accessorytype(int val);
#ifndef ARDUINO8266_SERVER_CPP
	void hap_init_homekit_server();
	bool hap_homekit_is_paired();
	void hap_restart_server();
#endif
	void hap_init_homekit_base_accessory();

	homekit_server_config_t* hap_get_server_config();
	bool hap_setup_final_step();
	