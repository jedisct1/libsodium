#ifndef crypto_shorthash_siphash24_H
#define crypto_shorthash_siphash24_H

#include "export.h"

#define crypto_shorthash_siphash24_BYTES 8
#define crypto_shorthash_siphash24_KEYBYTES 16

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
int crypto_shorthash_siphash24(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_shorthash_siphash24_ref crypto_shorthash_siphash24

#endif
