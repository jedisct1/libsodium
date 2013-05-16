#ifndef crypto_core_salsa2012_H
#define crypto_core_salsa2012_H

#include <stddef.h>
#include "export.h"

#define crypto_core_salsa2012_OUTPUTBYTES 64U
#define crypto_core_salsa2012_INPUTBYTES 16U
#define crypto_core_salsa2012_KEYBYTES 32U
#define crypto_core_salsa2012_CONSTBYTES 16U

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_core_salsa2012_outputbytes(void);

SODIUM_EXPORT
size_t crypto_core_salsa2012_inputbytes(void);

SODIUM_EXPORT
size_t crypto_core_salsa2012_keybytes(void);

SODIUM_EXPORT
size_t crypto_core_salsa2012_constbytes(void);

SODIUM_EXPORT
const char * crypto_core_salsa2012_primitive(void);

SODIUM_EXPORT
int crypto_core_salsa2012(unsigned char *,const unsigned char *,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_core_salsa2012_ref crypto_core_salsa2012

#endif
