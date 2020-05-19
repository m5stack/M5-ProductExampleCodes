#pragma once
#include "port_x.h"
#ifdef ARDUINO8266_SERVER_CPP
#include "homeintegration.h"

void hap_init_homekit_server();
void hap_homekit_loop();
bool hap_homekit_is_paired();
void hap_restart_server();
#endif
