#ifndef crypto_shorthash_H
#define crypto_shorthash_H

#include "crypto_shorthash_siphash24.h"

#define crypto_shorthash_BYTES crypto_shorthash_siphash24_BYTES
#define crypto_shorthash_KEYBYTES crypto_shorthash_siphash24_KEYBYTES
#define crypto_shorthash_PRIMITIVE "siphash24"

#ifdef __cplusplus
extern "C" {
#endif

int crypto_shorthash(unsigned char *out, const unsigned char *in,
                     unsigned long long inlen, const unsigned char *k);

#ifdef __cplusplus
}
#endif

#endif
