#ifndef crypto_core_salsa208_H
#define crypto_core_salsa208_H

#include <stddef.h>
#include "export.h"

#define crypto_core_salsa208_OUTPUTBYTES 64U
#define crypto_core_salsa208_INPUTBYTES 16U
#define crypto_core_salsa208_KEYBYTES 32U
#define crypto_core_salsa208_CONSTBYTES 16U

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_core_salsa208_outputbytes(void);

SODIUM_EXPORT
size_t crypto_core_salsa208_inputbytes(void);

SODIUM_EXPORT
size_t crypto_core_salsa208_keybytes(void);

SODIUM_EXPORT
size_t crypto_core_salsa208_constbytes(void);

SODIUM_EXPORT
const char * crypto_core_salsa208_primitive(void);

SODIUM_EXPORT
int crypto_core_salsa208(unsigned char *,const unsigned char *,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_core_salsa208_ref crypto_core_salsa208

#endif
