#ifndef crypto_xof_shake256_H
#define crypto_xof_shake256_H

#include "export.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_xof_shake256_BLOCKBYTES 136U
SODIUM_EXPORT
size_t crypto_xof_shake256_blockbytes(void);

#define crypto_xof_shake256_STATEBYTES 256U
SODIUM_EXPORT
size_t crypto_xof_shake256_statebytes(void);

#define crypto_xof_shake256_DOMAIN_STANDARD 0x1FU
SODIUM_EXPORT
unsigned char crypto_xof_shake256_domain_standard(void);

typedef struct CRYPTO_ALIGN(16) crypto_xof_shake256_state {
    unsigned char opaque[256];
} crypto_xof_shake256_state;

/* One-shot API */
SODIUM_EXPORT
int crypto_xof_shake256(unsigned char *out, size_t outlen, const unsigned char *in,
                        unsigned long long inlen) __attribute__((nonnull(1)));

/* Streaming API with standard domain */
SODIUM_EXPORT
int crypto_xof_shake256_init(crypto_xof_shake256_state *state) __attribute__((nonnull));

/* Streaming API with custom domain */
SODIUM_EXPORT
int crypto_xof_shake256_init_with_domain(crypto_xof_shake256_state *state, unsigned char domain)
    __attribute__((nonnull));

SODIUM_EXPORT
int crypto_xof_shake256_update(crypto_xof_shake256_state *state,
                               const unsigned char       *in,
                               unsigned long long         inlen) __attribute__((nonnull(1)));

SODIUM_EXPORT
int crypto_xof_shake256_squeeze(crypto_xof_shake256_state *state, unsigned char *out, size_t outlen)
    __attribute__((nonnull));

#ifdef __cplusplus
}
#endif

#endif
