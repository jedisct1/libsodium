#ifndef crypto_core_salsa20_H
#define crypto_core_salsa20_H

#include <stddef.h>
#include "export.h"

#define crypto_core_salsa20_OUTPUTBYTES 64U
#define crypto_core_salsa20_INPUTBYTES 16U
#define crypto_core_salsa20_KEYBYTES 32U
#define crypto_core_salsa20_CONSTBYTES 16U

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_core_salsa20_outputbytes(void);

SODIUM_EXPORT
size_t crypto_core_salsa20_inputbytes(void);

SODIUM_EXPORT
size_t crypto_core_salsa20_keybytes(void);

SODIUM_EXPORT
size_t crypto_core_salsa20_constbytes(void);

SODIUM_EXPORT
const char * crypto_core_salsa20_primitive(void);

SODIUM_EXPORT
int crypto_core_salsa20(unsigned char *,const unsigned char *,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_core_salsa20_ref crypto_core_salsa20

#endif
