#ifndef crypto_core_hsalsa20_H
#define crypto_core_hsalsa20_H

#include "export.h"

#define crypto_core_hsalsa20_OUTPUTBYTES 32
#define crypto_core_hsalsa20_INPUTBYTES 16
#define crypto_core_hsalsa20_KEYBYTES 32
#define crypto_core_hsalsa20_CONSTBYTES 16

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
int crypto_core_hsalsa20(unsigned char *,const unsigned char *,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_core_hsalsa20_ref2 crypto_core_hsalsa20

#endif
