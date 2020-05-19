#pragma once
#include <Arduino.h>

extern "C" {
#include "../homeintegration.h"
}
static String hap_pair_file_name;
void hap_storage_changed(char * szstorage, int bufsize) {



	SPIFFS.remove(hap_pair_file_name);
	File fsDAT = SPIFFS.open(hap_pair_file_name, "w+");
	if (!fsDAT) {
		Serial.println("Failed to open pair.dat");
		return;
	}
	fsDAT.write((uint8_t*)szstorage, bufsize);

	fsDAT.close();
}
void init_hap_storage(const char* filename) {
	//Serial.print("init_hap_storage");
	hap_pair_file_name = filename;
	if (!hap_pair_file_name.startsWith("/")) hap_pair_file_name = "/" + hap_pair_file_name;
	File fsDAT = SPIFFS.open(hap_pair_file_name, "r");

	if (!fsDAT) {
		//Serial.println("Failed to read pair.dat");
		SPIFFS.format();

	}
	int size = hap_get_storage_size_ex();
	char* buf = new char[size];
	memset(buf, 0xff, size);
	if (fsDAT)
		fsDAT.readBytes(buf, size);

	hap_init_storage_ex(buf, size);
	if (fsDAT)
		fsDAT.close();
	delete[]buf;

	set_callback_storage_change(hap_storage_changed);
}