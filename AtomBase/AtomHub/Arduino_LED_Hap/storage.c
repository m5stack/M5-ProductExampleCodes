#include <string.h>
#include <ctype.h>
#include "debug.h"
#include "crypto.h"
#include "pairing.h"
#include "port.h"


//#include "storage_ex.h"

#pragma GCC diagnostic ignored "-Wunused-value"

#ifndef SPIFLASH_BASE_ADDR
#define SPIFLASH_BASE_ADDR 0x200000
#endif

#define MAGIC_OFFSET           0
#define ACCESSORY_ID_OFFSET    4
#define ACCESSORY_KEY_OFFSET   32
#define PAIRINGS_OFFSET        128

#define MAGIC_ADDR           (SPIFLASH_BASE_ADDR + MAGIC_OFFSET)
#define ACCESSORY_ID_ADDR    (SPIFLASH_BASE_ADDR + ACCESSORY_ID_OFFSET)
#define ACCESSORY_KEY_ADDR   (SPIFLASH_BASE_ADDR + ACCESSORY_KEY_OFFSET)
#define PAIRINGS_ADDR        (SPIFLASH_BASE_ADDR + PAIRINGS_OFFSET)

#ifndef MAX_PAIRINGS
#define MAX_PAIRINGS 16
#endif

#ifndef ACCESSORY_ID_SIZE
#define ACCESSORY_ID_SIZE   17
#endif
#define ACCESSORY_KEY_SIZE  64

const char magic1[] = "HAP";

typedef struct {
	char magic[sizeof(magic1)];
	byte permissions;
	char device_id[36];
	byte device_public_key[32];

	byte _reserved[7]; // align record to be 80 bytes
} pairing_data_t;



#ifdef EX_STORAGE_CHAR

#define MAGIC_ADDR_EX 0
#define ACCESSORY_ID_ADDR_EX (0 + ACCESSORY_ID_OFFSET)
#define ACCESSORY_KEY_ADDR_EX   (0 + ACCESSORY_KEY_OFFSET)
#define PAIRINGS_ADDR_EX        (0 + PAIRINGS_OFFSET)
char ex_storage[PAIRINGS_OFFSET + MAX_PAIRINGS * sizeof(pairing_data_t) + 1] = { 0xFF };

int init_storage_ex(char* szdata, int size) {
	int actualsize = size > sizeof(ex_storage) ? sizeof(ex_storage) : size;
	INFO("init_storage_ex size 0x%x", actualsize);
	memcpy((void *)&ex_storage[0], szdata, actualsize);
	return actualsize;
}
char * get_ex_storage() {
	return ex_storage;
}
int get_ex_storage_size() {
	return sizeof(ex_storage);
}
typedef void(*callback_function)(void);

static callback_function callbackstorage = NULL;

void set_callback_storage(callback_function fn) {
	callbackstorage = fn;
}
void on_storage_change() {

	if (callbackstorage) {
		callbackstorage();

	}
}
#endif


int homekit_storage_reset() {
	byte blank[sizeof(magic1)];
#ifdef EX_STORAGE_CHAR
	// blabla();
	// memset((void*)ex_storage,0,sizeof(ex_storage));
	memcpy((void *)&ex_storage[MAGIC_ADDR_EX], blank, sizeof(blank));
	on_storage_change();
#else
	if (!spiflash_write(MAGIC_ADDR, blank, sizeof(blank))) {
		ERROR("Failed to reset flash");
		return -1;
	}
#endif
	return 0;
}


int homekit_storage_init() {
	char magic[sizeof(magic1)];
	memset(magic, 0, sizeof(magic));
#ifdef EX_STORAGE_CHAR
	memcpy(magic, (void *)&ex_storage[MAGIC_ADDR_EX], sizeof(magic));
#else
	if (!spiflash_read(MAGIC_ADDR, (byte *)magic, sizeof(magic))) {
		ERROR("Failed to read flash magic");
	}
#endif
	if (strncmp(magic, magic1, sizeof(magic1))) {
		INFO("Formatting flash at 0x%x", SPIFLASH_BASE_ADDR);
#ifdef EX_STORAGE_CHAR
		
		memset((void *)&ex_storage[0], 0xff, sizeof(ex_storage));
		//on_storage_change();
		//INFO("storage %s",ex_storage);
#else
		if (!spiflash_erase_sector(SPIFLASH_BASE_ADDR)) {
			ERROR("Failed to erase flash");
			return -1;
		}
#endif
		strncpy(magic, magic1, sizeof(magic));
#ifdef EX_STORAGE_CHAR

		if (!memcpy((void *)&ex_storage[MAGIC_ADDR_EX], (void *)magic, sizeof(magic))) {
			ERROR("Failed to initialize ex_buffer");
			return -1;
		}
		//INFO("storage %s",ex_storage);
		on_storage_change();
		// INFO("homekit  storage ex init done");
#else
		if (!spiflash_write(MAGIC_ADDR, (byte *)magic, sizeof(magic))) {
			ERROR("Failed to initialize flash");
			return -1;
		}
#endif
		// INFO("homekit_storage_init done");
		return 1;

	}


	return 0;
}


void homekit_storage_save_accessory_id(const char *accessory_id) {
#ifdef EX_STORAGE_CHAR
	memcpy((void *)&ex_storage[ACCESSORY_ID_ADDR_EX], (byte *)accessory_id, strlen(accessory_id));
	on_storage_change();
#else

	if (!spiflash_write(ACCESSORY_ID_ADDR, (byte *)accessory_id, strlen(accessory_id))) {
		ERROR("Failed to write accessory ID to flash");
	}
#endif
}


static char ishex(unsigned char c) {
	c = toupper(c);
	return isdigit(c) || (c >= 'A' && c <= 'F');
}

char *homekit_storage_load_accessory_id() {
	byte data[ACCESSORY_ID_SIZE + 1];
//	INFO("homekit  load_accessory_id");
#ifdef EX_STORAGE_CHAR
	memcpy((void*)data, &ex_storage[ACCESSORY_ID_ADDR_EX], sizeof(data));
#else
	if (!spiflash_read(ACCESSORY_ID_ADDR, data, sizeof(data))) {
		ERROR("Failed to read accessory ID from flash");
		return NULL;
	}
#endif

	if (!data[0])
		return NULL;
//	INFO("data  0x%s",(char*)data);
	data[sizeof(data) - 1] = 0;

	for (int i = 0; i < 17; i++) {
		if (i % 3 == 2) {
			if (data[i] != ':')
				return NULL;
		}
		else if (!ishex(data[i]))
			return NULL;
	}

	return strndup((char *)data, sizeof(data));
}

void homekit_storage_save_accessory_key(const ed25519_key *key) {
	byte key_data[ACCESSORY_KEY_SIZE];
	size_t key_data_size = sizeof(key_data);
	int r = crypto_ed25519_export_key(key, key_data, &key_data_size);
	if (r) {
		ERROR("Failed to export accessory key (code %d)", r);
		return;
	}
#ifdef EX_STORAGE_CHAR
	memcpy((void*)&ex_storage[ACCESSORY_KEY_ADDR_EX], key_data, key_data_size);
	on_storage_change();
#else
	if (!spiflash_write(ACCESSORY_KEY_ADDR, key_data, key_data_size)) {
		ERROR("Failed to write accessory key to flash");
		return;
	}
#endif
}

ed25519_key *homekit_storage_load_accessory_key() {

	byte key_data[ACCESSORY_KEY_SIZE];
#ifdef EX_STORAGE_CHAR
	memcpy(key_data, (void*)&ex_storage[ACCESSORY_KEY_ADDR_EX], sizeof(key_data));
#else
	if (!spiflash_read(ACCESSORY_KEY_ADDR, key_data, sizeof(key_data))) {
		ERROR("Failed to read accessory key from flash");
		return NULL;
	}
#endif
	// INFO("homekit  load_accessory_key");
	ed25519_key *key = crypto_ed25519_new();
	int r = crypto_ed25519_import_key(key, key_data, sizeof(key_data));
	if (r) {
		ERROR("Failed to import accessory key (code %d)", r);
		crypto_ed25519_free(key);
		return NULL;
	}

	return key;
}

// TODO: figure out alignment issues


bool homekit_storage_can_add_pairing() {
	pairing_data_t data;
	for (int i = 0; i < MAX_PAIRINGS; i++) {
#ifdef EX_STORAGE_CHAR
		memcpy((byte *)&data, (void*)&ex_storage[PAIRINGS_ADDR_EX + sizeof(data)*i], sizeof(data));
#else
		spiflash_read(PAIRINGS_ADDR + sizeof(data)*i, (byte *)&data, sizeof(data));
#endif
		if (strncmp(data.magic, magic1, sizeof(magic1)))
			return true;
	}
	return false;
}

static int compact_data() {
	byte *data = malloc(SPI_FLASH_SECTOR_SIZE);
#ifdef EX_STORAGE_CHAR
	memcpy((byte *)data, (void*)&ex_storage[0], SPI_FLASH_SECTOR_SIZE);
#else
	if (!spiflash_read(SPIFLASH_BASE_ADDR, data, SPI_FLASH_SECTOR_SIZE)) {
		free(data);
		ERROR("Failed to compact data: sector data read error");
		return -1;
	}
#endif
	int next_pairing_idx = 0;
	for (int i = 0; i < MAX_PAIRINGS; i++) {
		pairing_data_t *pairing_data = (pairing_data_t *)&data[PAIRINGS_OFFSET + sizeof(pairing_data_t)*i];
		if (!strncmp(pairing_data->magic, magic1, sizeof(magic1))) {
			if (i != next_pairing_idx) {
				memcpy(&data[PAIRINGS_ADDR + sizeof(pairing_data_t)*next_pairing_idx],
					pairing_data, sizeof(*pairing_data));
			}
			next_pairing_idx++;
		}
	}

	if (next_pairing_idx == MAX_PAIRINGS) {
		// We are full, no compaction possible, do not waste flash erase cycle
		return 0;
	}

	if (homekit_storage_reset()) {
		ERROR("Failed to compact data: error resetting flash");
		return -1;
	}
	if (homekit_storage_init() < 0) {
		ERROR("Failed to compact data: error initializing flash");
		return -1;
	}

#ifdef EX_STORAGE_CHAR
	memcpy((void*)&ex_storage[0], data, PAIRINGS_OFFSET + sizeof(pairing_data_t)*next_pairing_idx);
	on_storage_change();
#else
	if (!spiflash_write(SPIFLASH_BASE_ADDR, data, PAIRINGS_OFFSET + sizeof(pairing_data_t)*next_pairing_idx)) {
		ERROR("Failed to compact data: error writing compacted data");
		return -1;
	}
#endif
	return 0;
}

static int find_empty_block() {
	byte data[sizeof(pairing_data_t)];

	for (int i = 0; i < MAX_PAIRINGS; i++) {

#ifdef EX_STORAGE_CHAR
		memcpy(data, (void*)&ex_storage[PAIRINGS_ADDR_EX + sizeof(data)*i], sizeof(data));
#else
		spiflash_read(PAIRINGS_ADDR + sizeof(data)*i, data, sizeof(data));
#endif
		bool block_empty = true;
		for (int j = 0; j < sizeof(data); j++)
			if (data[j] != 0xff) {
				block_empty = false;
				break;
			}

		if (block_empty)
			return i;
	}

	return -1;
}

int homekit_storage_add_pairing(const char *device_id, const ed25519_key *device_key, byte permissions) {
	int next_block_idx = find_empty_block();
	if (next_block_idx == -1) {
		compact_data();
		next_block_idx = find_empty_block();
	}

	if (next_block_idx == -1) {
		ERROR("Failed to write pairing info to flash: max number of pairings");
		return -2;
	}

	pairing_data_t data;

	memset(&data, 0, sizeof(data));
	strncpy(data.magic, magic1, sizeof(data.magic));
	data.permissions = permissions;
	strncpy(data.device_id, device_id, sizeof(data.device_id));
	size_t device_public_key_size = sizeof(data.device_public_key);
	int r = crypto_ed25519_export_public_key(
		device_key, data.device_public_key, &device_public_key_size
	);
	if (r) {
		ERROR("Failed to export device public key (code %d)", r);
		return -1;
	}
#ifdef EX_STORAGE_CHAR
	memcpy((void*)&ex_storage[PAIRINGS_ADDR_EX + sizeof(data)*next_block_idx], (byte *)&data, sizeof(data));
	on_storage_change();
#else
	if (!spiflash_write(PAIRINGS_ADDR + sizeof(data)*next_block_idx, (byte *)&data, sizeof(data))) {
		ERROR("Failed to write pairing info to flash");
		return -1;
	}
#endif
	return 0;
}


int homekit_storage_update_pairing(const char *device_id, byte permissions) {
	pairing_data_t data;
	for (int i = 0; i < MAX_PAIRINGS; i++) {
#ifdef EX_STORAGE_CHAR
		memcpy((byte *)&data, (void*)&ex_storage[PAIRINGS_ADDR_EX + sizeof(data)*i], sizeof(data));
#else
		spiflash_read(PAIRINGS_ADDR + sizeof(data)*i, (byte *)&data, sizeof(data));
#endif
		if (strncmp(data.magic, magic1, sizeof(data.magic)))
			continue;

		if (!strncmp(data.device_id, device_id, sizeof(data.device_id))) {
			int r;

			ed25519_key *device_key = crypto_ed25519_new();
			r = crypto_ed25519_import_public_key(device_key, data.device_public_key, sizeof(data.device_public_key));
			if (r) {
				ERROR("Failed to import device public key (code %d)", r);
				crypto_ed25519_free(device_key);
				return -2;
			}

			r = homekit_storage_add_pairing(data.device_id, device_key, permissions);
			crypto_ed25519_free(device_key);
			if (r) {
				return -2;
			}

			memset(&data, 0, sizeof(data));
#ifdef EX_STORAGE_CHAR
			memcpy((void*)&ex_storage[PAIRINGS_ADDR_EX + sizeof(data)*i], (byte *)&data, sizeof(data));
			on_storage_change();
#else
			if (!spiflash_write(PAIRINGS_ADDR + sizeof(data)*i, (byte *)&data, sizeof(data))) {
				ERROR("Failed to update pairing: error erasing old record");
				return -2;
			}
#endif
			return 0;
		}
	}
	return -1;
}


int homekit_storage_remove_pairing(const char *device_id) {
	pairing_data_t data;
	for (int i = 0; i < MAX_PAIRINGS; i++) {
#ifdef EX_STORAGE_CHAR
		memcpy((byte *)&data, (void*)&ex_storage[PAIRINGS_ADDR_EX + sizeof(data)*i], sizeof(data));
#else
		spiflash_read(PAIRINGS_ADDR + sizeof(data)*i, (byte *)&data, sizeof(data));
#endif
		if (strncmp(data.magic, magic1, sizeof(data.magic)))
			continue;

		if (!strncmp(data.device_id, device_id, sizeof(data.device_id))) {
			memset(&data, 0, sizeof(data));
#ifdef EX_STORAGE_CHAR
			memcpy((void*)&ex_storage[PAIRINGS_ADDR_EX + sizeof(data)*i], (byte *)&data, sizeof(data));
			on_storage_change();
#else
			if (!spiflash_write(PAIRINGS_ADDR + sizeof(data)*i, (byte *)&data, sizeof(data))) {
				ERROR("Failed to remove pairing from flash");
				return -2;
			}
#endif
			return 0;
		}
	}
	return 0;
}


pairing_t *homekit_storage_find_pairing(const char *device_id) {
	pairing_data_t data;
	for (int i = 0; i < MAX_PAIRINGS; i++) {
#ifdef EX_STORAGE_CHAR
		memcpy((byte *)&data, (void*)&ex_storage[PAIRINGS_ADDR_EX + sizeof(data)*i], sizeof(data));
#else
		spiflash_read(PAIRINGS_ADDR + sizeof(data)*i, (byte *)&data, sizeof(data));
#endif
		if (strncmp(data.magic, magic1, sizeof(data.magic)))
			continue;

		if (!strncmp(data.device_id, device_id, sizeof(data.device_id))) {
			ed25519_key *device_key = crypto_ed25519_new();
			int r = crypto_ed25519_import_public_key(device_key, data.device_public_key, sizeof(data.device_public_key));
			if (r) {
				ERROR("Failed to import device public key (code %d)", r);
				return NULL;
			}

			pairing_t *pairing = pairing_new();
			pairing->id = i;
			pairing->device_id = strndup(data.device_id, sizeof(data.device_id));
			pairing->device_key = device_key;
			pairing->permissions = data.permissions;

			return pairing;
		}
	}

	return NULL;
}


typedef struct {
	int idx;
} pairing_iterator_t;


pairing_iterator_t *homekit_storage_pairing_iterator() {
	pairing_iterator_t *it = malloc(sizeof(pairing_iterator_t));
	it->idx = 0;
	return it;
}


void homekit_storage_pairing_iterator_free(pairing_iterator_t *iterator) {
	free(iterator);
}


pairing_t *homekit_storage_next_pairing(pairing_iterator_t *it) {
	pairing_data_t data;
	while (it->idx < MAX_PAIRINGS) {
		int id = it->idx++;
#ifdef EX_STORAGE_CHAR
		memcpy((byte *)&data, (void*)&ex_storage[PAIRINGS_ADDR_EX + sizeof(data)*id], sizeof(data));
#else
		spiflash_read(PAIRINGS_ADDR + sizeof(data)*id, (byte *)&data, sizeof(data));
#endif
		if (!strncmp(data.magic, magic1, sizeof(data.magic))) {
			ed25519_key *device_key = crypto_ed25519_new();
			int r = crypto_ed25519_import_public_key(device_key, data.device_public_key, sizeof(data.device_public_key));
			if (r) {
				ERROR("Failed to import device public key (code %d)", r);
				crypto_ed25519_free(device_key);
				it->idx++;
				continue;
			}

			pairing_t *pairing = pairing_new();
			pairing->id = id;
			pairing->device_id = strndup(data.device_id, sizeof(data.device_id));
			pairing->device_key = device_key;
			pairing->permissions = data.permissions;

			return pairing;
		}
	}

	return NULL;
}

