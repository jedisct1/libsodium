#ifndef crypto_hash_sha3_H
#define crypto_hash_sha3_H

#include <stddef.h>
#include <stdint.h>

#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

typedef struct CRYPTO_ALIGN(16) crypto_hash_sha3256_state {
    unsigned char opaque[256];
} crypto_hash_sha3256_state;

SODIUM_EXPORT
size_t crypto_hash_sha3256_statebytes(void);

#define crypto_hash_sha3256_BYTES 32U
SODIUM_EXPORT
size_t crypto_hash_sha3256_bytes(void);

SODIUM_EXPORT
int crypto_hash_sha3256(unsigned char *out, const unsigned char *in,
                        unsigned long long inlen) __attribute__ ((nonnull(1)));

SODIUM_EXPORT
int crypto_hash_sha3256_init(crypto_hash_sha3256_state *state)
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_hash_sha3256_update(crypto_hash_sha3256_state *state,
                               const unsigned char *in,
                               unsigned long long inlen)
            __attribute__ ((nonnull(1)));

SODIUM_EXPORT
int crypto_hash_sha3256_final(crypto_hash_sha3256_state *state,
                              unsigned char *out)
            __attribute__ ((nonnull));

typedef struct CRYPTO_ALIGN(16) crypto_hash_sha3512_state {
    unsigned char opaque[256];
} crypto_hash_sha3512_state;

SODIUM_EXPORT
size_t crypto_hash_sha3512_statebytes(void);

#define crypto_hash_sha3512_BYTES 64U
SODIUM_EXPORT
size_t crypto_hash_sha3512_bytes(void);

SODIUM_EXPORT
int crypto_hash_sha3512(unsigned char *out, const unsigned char *in,
                        unsigned long long inlen) __attribute__ ((nonnull(1)));

SODIUM_EXPORT
int crypto_hash_sha3512_init(crypto_hash_sha3512_state *state)
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_hash_sha3512_update(crypto_hash_sha3512_state *state,
                               const unsigned char *in,
                               unsigned long long inlen)
            __attribute__ ((nonnull(1)));

SODIUM_EXPORT
int crypto_hash_sha3512_final(crypto_hash_sha3512_state *state,
                              unsigned char *out)
            __attribute__ ((nonnull));

#ifdef __cplusplus
}
#endif

#endif
