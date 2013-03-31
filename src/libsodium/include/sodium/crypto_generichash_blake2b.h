#ifndef crypto_generichash_blake2b_H
#define crypto_generichash_blake2b_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
    
SODIUM_EXPORT extern int crypto_generichash_blake2b_ref(unsigned char *out, const unsigned char *in,
                                          const unsigned char *key,
                                          size_t outlen, unsigned long long inlen,
                                          size_t keylen);

#ifdef __cplusplus
}
#endif

#define crypto_generichash_blake2b crypto_generichash_blake2b_ref
#define crypto_generichash_blake2b_IMPLEMENTATION
#ifndef crypto_generichash_blake2b_ref_VERSION
#define crypto_generichash_blake2b_ref_VERSION "-"
#endif
#define crypto_generichash_blake2b_VERSION crypto_generichash_blake2b_ref_VERSION

#endif
