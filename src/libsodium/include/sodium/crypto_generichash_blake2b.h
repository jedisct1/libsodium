#ifndef crypto_generichash_blake2b_H
#define crypto_generichash_blake2b_H

#include <stdlib.h>

#define crypto_generichash_blake2b_BYTES_MIN     1U
#define crypto_generichash_blake2b_BYTES_MAX    64U
#define crypto_generichash_blake2b_KEYBYTES_MIN  0U
#define crypto_generichash_blake2b_KEYBYTES_MAX 64U

#ifdef __cplusplus
extern "C" {
#endif
int crypto_generichash_blake2b_ref(unsigned char *out,
                                   const unsigned char *in,
                                   const unsigned char *key,
                                   size_t outlen,
                                   unsigned long long inlen,
                                   size_t keylen);

#ifdef __cplusplus
}
#endif

#define crypto_generichash_blake2b crypto_generichash_blake2b_ref

#endif
