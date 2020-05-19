/* Generated wolfSSL user_settings.h file for Arduino */
#ifndef ARDUINO_USER_SETTINGS_H
#define ARDUINO_USER_SETTINGS_H

/* Platform */
#define WOLFSSL_ARDUINO

/* Math library (remove this to use normal math)*/

//YK
//#define USE_FAST_MATH
//#define TFM_NO_ASM

/* RNG DEFAULT !!FOR TESTING ONLY!! */
/* comment out the error below to get started w/ bad entropy source
 * This will need fixed before distribution but is OK to test with */
//error "needs solved, see: https://www.wolfssl.com/docs/porting-guide/"

#define WOLFCRYPT_HAVE_SRP 
#define WOLFSSL_SHA512 
#define WOLFSSL_BASE64_ENCODE 
#define NO_MD5 
#define NO_SHA 
#define HAVE_HKDF 
#define HAVE_CHACHA 
#define HAVE_POLY1305 
#define HAVE_ED25519 
#define HAVE_CURVE25519 
#define NO_SESSION_CACHE 
//#define USE_WOLFSSL_MEMORY 
#define RSA_LOW_MEM 
#define GCM_SMALL 
#define USE_SLOW_SHA512 
#define WOLFCRYPT_ONLY 
//#define WOLFSSL_ESPIDF

#ifdef ARDUINO 
#include <esp_system.h>

static inline int hwrand_generate_block(uint8_t *buf, size_t len) {
    int i;
    for (i=0; i+4 < len; i+=4) {
        *((uint32_t*)buf) = esp_random();
        buf += 4;
    }
    if (i < len) {
        uint32_t r = esp_random();
        while (i < len) {
            *buf++ = r;
            r >>= 8;
            i++;
        }
    }

    return 0;
}
#define CUSTOM_RAND_GENERATE_BLOCK hwrand_generate_block
#endif




//#define CUSTOM_RAND_GENERATE_SEED



//#define WOLFSSL_ESPWROOM32
//#define CURVE25519_SMALL 
//#define ED25519_SMALL 
//#define WOLFSSL_GENSEED_FORTEST
//#define FREERTOS_TCP

//#define HAVE_ED25519_KEY_EXPORT
//#define HAVE_ED25519_KEY_IMPORT
//#define NO_DES
//#define NO_DES3
//#define NO_DSA
//#define TIME_OVERRIDES



#define NO_ASN_TIME
#define XTIME time_t 
#define XGMTIME 

#define WC_SHA_DIGEST_SIZE 20
#endif /* ARDUINO_USER_SETTINGS_H */
