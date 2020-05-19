#pragma once
#include "port_x.h"

#if defined(ARDUINO8266_SERVER)

#ifdef __cplusplus
extern "C" {
#endif
	void mdns_init();

	// Build and advertise an appropriate linked set of PTR/TXT/SRV/A records for the parameters provided
	// This is a simple canned way to build a set of records for a single service that will
	// be advertised whenever the device is given an IP address by WiFi

	typedef enum {
		mdns_TCP,
		mdns_UDP,
		mdns_Browsable        // see RFC6763:11 - adds a standard record that lets browsers find the service without needing to know its name
	} mdns_flags;

	// Clear all records
	void mdns_clear();


	void mdns_add_facility(const char* instanceName,   // Short user-friendly instance name, should NOT include serial number/MAC/etc
		const char* serviceName,    // Must be registered, _name, (see RFC6335 5.1 & 5.2)
		const char* addText,        // Should be <key>=<value>, or "" if unused (see RFC6763 6.3)
		mdns_flags  flags,          // TCP or UDP plus browsable
		unsigned int       onPort,         // port number
		unsigned int       ttl             // time-to-live, seconds
	);


	// Low-level RR builders for rolling your own
	/*
	void mdns_add_PTR(const char* rKey, u32_t ttl, const char* nameStr);
	void mdns_add_SRV(const char* rKey, u32_t ttl, u16_t rPort, const char* targname);
	void mdns_add_TXT(const char* rKey, u32_t ttl, const char* txtStr);
	void mdns_add_A(const char* rKey, u32_t ttl, const ip4_addr_t *addr);
	#if LWIP_IPV6
	void mdns_add_AAAA(const char* rKey, u32_t ttl, const ip6_addr_t *addr);
	#endif
	*/
	void mdns_init8266(const char* instanceName, const char* serviceName, const char* serviceProto, unsigned int port, unsigned int  ttl);
	void mdns_TXT_append(char* txt, unsigned int txt_size, const char* record, unsigned int record_size);
	void mdns_TXT_append_ex(const char* serviceName, const char* serviceProto, char* txt, const char* record);
	void  mdns_stop();
#ifdef __cplusplus
}
#endif
#endif