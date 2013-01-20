#ifndef crypto_core_hsalsa20_H
#define crypto_core_hsalsa20_H

#define crypto_core_hsalsa20_ref2_OUTPUTBYTES 32
#define crypto_core_hsalsa20_ref2_INPUTBYTES 16
#define crypto_core_hsalsa20_ref2_KEYBYTES 32
#define crypto_core_hsalsa20_ref2_CONSTBYTES 16
#ifdef __cplusplus
#include <string>
extern "C" {
#endif
extern int crypto_core_hsalsa20_ref2(unsigned char *,const unsigned char *,const unsigned char *,const unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_core_hsalsa20 crypto_core_hsalsa20_ref2
#define crypto_core_hsalsa20_OUTPUTBYTES crypto_core_hsalsa20_ref2_OUTPUTBYTES
#define crypto_core_hsalsa20_INPUTBYTES crypto_core_hsalsa20_ref2_INPUTBYTES
#define crypto_core_hsalsa20_KEYBYTES crypto_core_hsalsa20_ref2_KEYBYTES
#define crypto_core_hsalsa20_CONSTBYTES crypto_core_hsalsa20_ref2_CONSTBYTES
#define crypto_core_hsalsa20_IMPLEMENTATION "crypto_core/hsalsa20/ref2"
#ifndef crypto_core_hsalsa20_ref2_VERSION
#define crypto_core_hsalsa20_ref2_VERSION "-"
#endif
#define crypto_core_hsalsa20_VERSION crypto_core_hsalsa20_ref2_VERSION

#endif
