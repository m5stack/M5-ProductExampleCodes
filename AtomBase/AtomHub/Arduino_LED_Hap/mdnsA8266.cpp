#include "port_x.h"

#if defined(ARDUINO8266_SERVER)
#include <Arduino.h>
#include "mdnsA8266.h"


#include "debug.h"

//#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
//#include <WiFiClient.h>
extern "C" void mdns_init() {

};
esp8266::MDNSImplementation::MDNSResponder* MDNS1;
LOCAL os_timer_t mdnstimer;
void mdnscallback(void * arg) {
	//INFO("mdnscallback");
	MDNS1->update();
}
extern "C" void ICACHE_FLASH_ATTR mdns_add_facility(const char* instanceName,   // Short user-friendly instance name, should NOT include serial number/MAC/etc
	const char* serviceName,    // Must be registered, _name, (see RFC6335 5.1 & 5.2)
	const char* addText,        // Should be <key>=<value>, or "" if unused (see RFC6763 6.3)
	mdns_flags  flags,          // TCP or UDP plus browsable
	unsigned int       onPort,         // port number
	unsigned int       ttl             // time-to-live, seconds
) 
{
	//INFO("instance %s", instanceName);
	//INFO("service %s", serviceName);
	//INFO("addText %s", addText);
	//INFO("port %d", onPort);

	//MDNS.begin(serviceName);
};
extern "C" void ICACHE_FLASH_ATTR mdns_TXT_append_ex(const char* serviceName, const char* serviceProto,char* txt, const char* record) {
	//INFO("append txt %s", txt);
	//INFO("append record %s", record);
	//MDNS.addServiceTxt(serviceName, serviceProto, txt, record);
	MDNS1->addServiceTxt(serviceName, serviceProto, txt, record);
}
extern "C" void ICACHE_FLASH_ATTR mdns_TXT_append(char* txt, unsigned int txt_size, const char* record, unsigned int record_size) {
	//INFO("append txt %s", txt);
	//INFO("append record %s", record);

}
extern "C"	void  ICACHE_FLASH_ATTR mdns_init8266(const char* instanceName, const char* serviceName, const char* serviceProto, unsigned int port, unsigned int  ttl){
	MDNS1= new esp8266::MDNSImplementation::MDNSResponder();
	
	MDNS1->begin(instanceName, INADDR_ANY, ttl);
	MDNS1->addService(serviceName, serviceProto, port);
	os_timer_disarm(&mdnstimer);
	os_timer_setfn(&mdnstimer, (os_timer_func_t *)mdnscallback, (void *)NULL);
	os_timer_arm(&mdnstimer, 2000, true);

	//MDNS.begin(instanceName, INADDR_ANY, ttl);
	//MDNS.addService(serviceName, serviceProto, port);
}
extern "C"	void ICACHE_FLASH_ATTR mdns_stop() {
	//INFO("mdns_clear");
	os_timer_disarm(&mdnstimer);
}

extern "C"	void ICACHE_FLASH_ATTR mdns_clear() {
	INFO("mdns_clear");
}


#endif