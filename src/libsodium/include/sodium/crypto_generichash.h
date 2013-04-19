#ifndef crypto_generichash_H
#define crypto_generichash_H

#include "crypto_generichash_blake2b.h"

#define crypto_generichash_BYTES_MIN crypto_generichash_blake2b_BYTES_MIN
#define crypto_generichash_BYTES_MAX crypto_generichash_blake2b_BYTES_MAX
#define crypto_generichash_KEYBYTES_MIN crypto_generichash_blake2b_KEYBYTES_MIN
#define crypto_generichash_KEYBYTES_MAX crypto_generichash_blake2b_KEYBYTES_MAX
#define crypto_generichash_PRIMITIVE "blake2b"

int crypto_generichash(unsigned char *out, const unsigned char *in,
                       const unsigned char *key,
                       size_t outlen, unsigned long long inlen,
                       size_t keylen);

#endif

