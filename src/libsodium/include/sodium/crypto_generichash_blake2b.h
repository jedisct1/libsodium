#ifndef crypto_generichash_blake2b_H
#define crypto_generichash_blake2b_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "export.h"

#define crypto_generichash_blake2b_BYTES         32U
#define crypto_generichash_blake2b_BYTES_MIN     16U
#define crypto_generichash_blake2b_BYTES_MAX     64U
#define crypto_generichash_blake2b_KEYBYTES      32U
#define crypto_generichash_blake2b_KEYBYTES_MIN  16U
#define crypto_generichash_blake2b_KEYBYTES_MAX  64U
#define crypto_generichash_blake2b_BLOCKBYTES   128U

#if defined(_MSC_VER)
# define CRYPTO_ALIGN(x) __declspec(align(x))
#else
# define CRYPTO_ALIGN(x) __attribute__((aligned(x)))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)
CRYPTO_ALIGN(64) typedef struct crypto_generichash_blake2b_state {
    uint64_t h[8];
    uint64_t t[2];
    uint64_t f[2];
    uint8_t  buf[2 * crypto_generichash_blake2b_BLOCKBYTES];
    size_t   buflen;
    uint8_t  last_node;
} crypto_generichash_blake2b_state;
#pragma pack(pop)

SODIUM_EXPORT
size_t crypto_generichash_blake2b_bytes_min(void);

SODIUM_EXPORT
size_t crypto_generichash_blake2b_bytes_max(void);

SODIUM_EXPORT
size_t crypto_generichash_blake2b_keybytes_min(void);

SODIUM_EXPORT
size_t crypto_generichash_blake2b_keybytes_max(void);

SODIUM_EXPORT
size_t crypto_generichash_blake2b_blockbytes(void);

SODIUM_EXPORT
const char * crypto_generichash_blake2b_blockbytes_primitive(void);

SODIUM_EXPORT
int crypto_generichash_blake2b(unsigned char *out, size_t outlen,
                               const unsigned char *in,
                               unsigned long long inlen,
                               const unsigned char *key, size_t keylen);

SODIUM_EXPORT
int crypto_generichash_blake2b_init(crypto_generichash_blake2b_state *state,
                                    const unsigned char *key,
                                    const size_t keylen, const size_t outlen);

SODIUM_EXPORT
int crypto_generichash_blake2b_update(crypto_generichash_blake2b_state *state,
                                      const unsigned char *in,
                                      unsigned long long inlen);

SODIUM_EXPORT
int crypto_generichash_blake2b_final(crypto_generichash_blake2b_state *state,
                                     unsigned char *out,
                                     const size_t outlen);

#ifdef __cplusplus
}
#endif

#define crypto_generichash_blake2b_ref crypto_generichash_blake2b
#define crypto_generichash_blake2b_ref_init crypto_generichash_blake2b_init
#define crypto_generichash_blake2b_ref_update crypto_generichash_blake2b_update
#define crypto_generichash_blake2b_ref_final crypto_generichash_blake2b_final

#endif
