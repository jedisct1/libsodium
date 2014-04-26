#ifndef crypto_shorthash_siphash24_H
#define crypto_shorthash_siphash24_H

#include <stddef.h>
#include "export.h"

#define crypto_shorthash_siphash24_BYTES 8U
#define crypto_shorthash_siphash24_KEYBYTES 16U

#ifdef __cplusplus
# if __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_shorthash_siphash24_bytes(void);

SODIUM_EXPORT
size_t crypto_shorthash_siphash24_keybytes(void);

SODIUM_EXPORT
const char * crypto_shorthash_siphash24_primitive(void);

SODIUM_EXPORT
int crypto_shorthash_siphash24(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_shorthash_siphash24_ref crypto_shorthash_siphash24

#endif
