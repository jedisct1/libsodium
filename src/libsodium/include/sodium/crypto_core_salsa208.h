#ifndef crypto_core_salsa208_H
#define crypto_core_salsa208_H

#include "export.h"

#define crypto_core_salsa208_OUTPUTBYTES 64U
#define crypto_core_salsa208_INPUTBYTES 16U
#define crypto_core_salsa208_KEYBYTES 32U
#define crypto_core_salsa208_CONSTBYTES 16U

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
int crypto_core_salsa208(unsigned char *,const unsigned char *,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_core_salsa208_ref crypto_core_salsa208

#endif
