#ifndef crypto_xof_shake128_H
#define crypto_xof_shake128_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
#    ifdef __GNUC__
#        pragma GCC diagnostic ignored "-Wlong-long"
#    endif
extern "C" {
#endif

#define crypto_xof_shake128_BLOCKBYTES 168U
SODIUM_EXPORT
size_t crypto_xof_shake128_blockbytes(void);

#define crypto_xof_shake128_STATEBYTES 256U
SODIUM_EXPORT
size_t crypto_xof_shake128_statebytes(void);

#define crypto_xof_shake128_DOMAIN_STANDARD 0x1FU
SODIUM_EXPORT
unsigned char crypto_xof_shake128_domain_standard(void);

typedef struct CRYPTO_ALIGN(16) crypto_xof_shake128_state {
    unsigned char opaque[256];
} crypto_xof_shake128_state;

SODIUM_EXPORT
int crypto_xof_shake128(unsigned char *out, size_t outlen, const unsigned char *in,
                        unsigned long long inlen) __attribute__((nonnull(1)));

SODIUM_EXPORT
int crypto_xof_shake128_init(crypto_xof_shake128_state *state) __attribute__((nonnull));

SODIUM_EXPORT
int crypto_xof_shake128_init_with_domain(crypto_xof_shake128_state *state, unsigned char domain)
    __attribute__((nonnull));

SODIUM_EXPORT
int crypto_xof_shake128_update(crypto_xof_shake128_state *state,
                               const unsigned char       *in,
                               unsigned long long         inlen) __attribute__((nonnull(1)));

SODIUM_EXPORT
int crypto_xof_shake128_squeeze(crypto_xof_shake128_state *state, unsigned char *out, size_t outlen)
    __attribute__((nonnull));

#ifdef __cplusplus
}
#endif

#endif
