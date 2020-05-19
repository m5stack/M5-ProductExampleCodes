#include <stdarg.h>
#include <port_x.h>

#ifdef ESP_OPEN_RTOS

#include <string.h>
#include <esp/hwrand.h>
#include <espressif/esp_common.h>
#include <esplibs/libmain.h>
#include "mdnsresponder.h"

#ifndef MDNS_TTL
#define MDNS_TTL 4500
#endif

uint32_t homekit_random() {
    return hwrand();
}

void homekit_random_fill(uint8_t *data, size_t size) {
    hwrand_fill(data, size);
}

void homekit_system_restart() {
    sdk_system_restart();
}

void homekit_overclock_start() {
    sdk_system_overclock();
}

void homekit_overclock_end() {
    sdk_system_restoreclock();
}

static char mdns_instance_name[65] = {0};
static char mdns_txt_rec[128] = {0};
static int mdns_port = 80;

void homekit_mdns_init() {
    mdns_init();
}
void homekit_mdns_add_txt_ex(const char *key, const char *val) {

}
void homekit_mdns_configure_init(const char *instance_name, int port) {
    strncpy(mdns_instance_name, instance_name, sizeof(mdns_instance_name));
    mdns_txt_rec[0] = 0;
    mdns_port = port;
}

void homekit_mdns_add_txt(const char *key, const char *format, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, format);

    char value[128];
    int value_len = vsnprintf(value, sizeof(value), format, arg_ptr);

    va_end(arg_ptr);

    if (value_len && value_len < sizeof(value)-1) {
        char buffer[128];
        int buffer_len = snprintf(buffer, sizeof(buffer), "%s=%s", key, value);

        if (buffer_len < sizeof(buffer)-1)
            mdns_TXT_append(mdns_txt_rec, sizeof(mdns_txt_rec), buffer, buffer_len);
    }
}

void homekit_mdns_configure_finalize() {
    mdns_clear();
    mdns_add_facility(mdns_instance_name, "_hap", mdns_txt_rec, mdns_TCP, mdns_port, MDNS_TTL);

    printf("mDNS announcement: Name=%s %s Port=%d TTL=%d\n",
           mdns_instance_name, mdns_txt_rec, mdns_port, MDNS_TTL);
}

#endif

#ifdef ESP_IDF

#include <string.h>
#include <mdns.h>

uint32_t homekit_random() {
    return esp_random();
}

void homekit_random_fill(uint8_t *data, size_t size) {
    uint32_t x;
    for (int i=0; i<size; i+=sizeof(x)) {
        x = esp_random();
        memcpy(data+i, &x, (size-i >= sizeof(x)) ? sizeof(x) : size-i);
    }
}

void homekit_system_restart() {
    esp_restart();
}

void homekit_overclock_start() {
}

void homekit_overclock_end() {
}

void homekit_mdns_init() {
    mdns_init();
}
void homekit_mdns_add_txt_ex(const char *key, const char *val) {

}
void homekit_mdns_configure_init(const char *instance_name, int port) {
    mdns_hostname_set(instance_name);
    mdns_instance_name_set(instance_name);
    mdns_service_add(instance_name, "_hap", "_tcp", port, NULL, 0);
}

void homekit_mdns_add_txt(const char *key, const char *format, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, format);

    char value[128];
    int value_len = vsnprintf(value, sizeof(value), format, arg_ptr);

    va_end(arg_ptr);

    if (value_len && value_len < sizeof(value)-1) {
        mdns_service_txt_item_set("_hap", "_tcp", key, value);
    }
}

void homekit_mdns_configure_finalize() {
    /*
    printf("mDNS announcement: Name=%s %s Port=%d TTL=%d\n",
           name->value.string_value, txt_rec, PORT, 0);
    */
}

#endif


#if defined(ARDUINO) && defined(ESP32)
#include <Arduino.h>
#include <string.h>
#include <mdns.h>



uint32_t homekit_random() {
	return esp_random();
}

void homekit_random_fill(uint8_t *data, size_t size) {
	uint32_t x;
	for (int i = 0; i < size; i += sizeof(x)) {
		x = esp_random();
		memcpy(data + i, &x, (size - i >= sizeof(x)) ? sizeof(x) : size - i);
	}
}

void homekit_system_restart() {
	esp_restart();
}

void homekit_overclock_start() {
}

void homekit_overclock_end() {
}

void homekit_mdns_init() {
	mdns_init();
}
void homekit_mdns_add_txt_ex(const char *key, const char *val) {

}
void homekit_mdns_configure_init(const char *instance_name, int port) {
	mdns_hostname_set(instance_name);
	mdns_instance_name_set(instance_name);
	mdns_service_add(instance_name, "_hap", "_tcp", port, NULL, 0);
}

void homekit_mdns_add_txt(const char *key, const char *format, ...) {
	va_list arg_ptr;
	va_start(arg_ptr, format);

	char value[128];
	int value_len = vsnprintf(value, sizeof(value), format, arg_ptr);

	va_end(arg_ptr);

	if (value_len && value_len < sizeof(value) - 1) {
		mdns_service_txt_item_set("_hap", "_tcp", key, value);
	}
}

void homekit_mdns_configure_finalize() {
	/*
	printf("mDNS announcement: Name=%s %s Port=%d TTL=%d\n",
		   name->value.string_value, txt_rec, PORT, 0);
	*/
}
#endif

#if defined(ARDUINO) && defined(ESP8266) && !defined(ARDUINO8266_SERVER_CPP)

#include <string.h>
//#include <esp/hwrand.h>
//#include <espressif/esp_common.h>
//#include <esplibs/libmain.h>


#include "types.h"
#include "Arduino.h"
#include "user_interface.h"
#include "mdnsA8266.h"

#ifndef MDNS_TTL
#define MDNS_TTL 4500
#endif

uint32_t ICACHE_FLASH_ATTR homekit_random() {
	//return hwrand();
	return 0;// hwrand_fill'
}

void ICACHE_FLASH_ATTR homekit_random_fill(uint8_t *data, size_t size) {
	//hwrand_fill(data, size);
}

void ICACHE_FLASH_ATTR homekit_system_restart() {
	//sdk_system_restart();
}

void ICACHE_FLASH_ATTR homekit_overclock_start() {
	//sdk_system_overclock();
	system_update_cpu_freq(SYS_CPU_160MHZ);
	//ets_intr_lock();
}

void homekit_overclock_end() {
	//sdk_system_restoreclock();
//	system_update_cpu_freq(80);
	//ets_intr_unlock();
}

//static char mdns_instance_name[65] = { 0 };
//static char mdns_txt_rec[128] = { 0 };
static int mdns_port = 80;
const char * mdns_base_service = "hap";
const char * mdns_base_service_proto = "tcp";
void homekit_mdns_init() {
	mdns_init();
}

void ICACHE_FLASH_ATTR homekit_mdns_configure_init(const char *instance_name, int port) {
	//strncpy(mdns_instance_name, instance_name, sizeof(mdns_instance_name));
	//mdns_txt_rec[0] = 0;
	mdns_port = port;
	mdns_init8266(instance_name, mdns_base_service, mdns_base_service_proto, port, MDNS_TTL);
}
void homekit_mdns_stop() {
	mdns_stop();
}
void ICACHE_FLASH_ATTR homekit_mdns_add_txt_ex(const char *key, const char *val) {
	//mdns_TXT_append_ex(key, val);
}
void ICACHE_FLASH_ATTR homekit_mdns_add_txt(const char *key, const char *format, ...) {
	
	va_list arg_ptr;
	va_start(arg_ptr, format);

	char value[128];
	int value_len = vsnprintf(value, sizeof(value), format, arg_ptr);

	va_end(arg_ptr);
	
	if (value_len && value_len < sizeof(value) - 1) {
		mdns_TXT_append_ex(mdns_base_service, mdns_base_service_proto, key, value );
		//char buffer[128];
		//int buffer_len = snprintf(buffer, sizeof(buffer), "%s=%s", key, value);

		//if (buffer_len < sizeof(buffer) - 1)
		//	mdns_TXT_append(mdns_base_service, mdns_base_service_proto, key, buffer, );
	}
	
}

void ICACHE_FLASH_ATTR homekit_mdns_configure_finalize() {
	//mdns_clear();
	//mdns_add_facility(mdns_instance_name, mdns_base_service, mdns_txt_rec, mdns_TCP, mdns_port, MDNS_TTL);

	//printf("mDNS announcement: Name=%s %s Port=%d TTL=%d\n",
	//	mdns_instance_name, mdns_txt_rec, mdns_port, MDNS_TTL);
}

#endif


#if defined(ARDUINO) && defined(ESP8266) && defined(ARDUINO8266_SERVER_CPP)

#include <string.h>



#include "types.h"
#include "Arduino.h"
#include "user_interface.h"
#include "debug.h"

#ifndef MDNS_TTL
#define MDNS_TTL 4500
#endif
	uint32_t homekit_random() {
		return os_random();

	}

	void homekit_random_fill(uint8_t *data, size_t size) {
	//	INFO("os_get_random");
		os_get_random(data, size);
		/*
		    uint32_t x;
    for (int i=0; i<size; i+=sizeof(x)) {
        x = rand();//esp_random();
      memcpy(data+i, &x, (size-i >= sizeof(x)) ? sizeof(x) : size-i);
    }
	*/
	}
	void  homekit_system_restart() {
		//sdk_system_restart();
	}

	void  homekit_overclock_start() {
		//sdk_system_overclock();
		system_update_cpu_freq(SYS_CPU_160MHZ);
		//ets_intr_lock();
	}

	void homekit_overclock_end() {
		//sdk_system_restoreclock();
		system_update_cpu_freq(80);
		//ets_intr_unlock();
	}

	void homekit_mdns_init() {
		mdns_init();
	}

	void homekit_mdns_configure_init(const char *instance_name, int port) {
		mdns_hostname_set(instance_name);
		mdns_instance_name_set(instance_name);
		mdns_service_add(instance_name, "_hap", "_tcp", port, NULL, 0);
	}

	void homekit_mdns_add_txt(const char *key, const char *format, ...) {
		va_list arg_ptr;
		va_start(arg_ptr, format);

		char value[128];
		int value_len = vsnprintf(value, sizeof(value), format, arg_ptr);

		va_end(arg_ptr);

		if (value_len && value_len < sizeof(value) - 1) {
			mdns_service_txt_item_set("_hap", "_tcp", key, value);
		}
	}


#endif