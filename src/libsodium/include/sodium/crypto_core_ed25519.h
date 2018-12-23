#ifndef crypto_core_ed25519_H
#define crypto_core_ed25519_H

#include <stddef.h>
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_core_ed25519_BYTES 32
SODIUM_EXPORT
size_t crypto_core_ed25519_bytes(void);

#define crypto_core_ed25519_UNIFORMBYTES 32
SODIUM_EXPORT
size_t crypto_core_ed25519_uniformbytes(void);

#define crypto_core_ed25519_SCALARBYTES 32
SODIUM_EXPORT
size_t crypto_core_ed25519_scalarbytes(void);

SODIUM_EXPORT
int crypto_core_ed25519_is_valid_point(const unsigned char *p)
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_core_ed25519_add(unsigned char *r,
                            const unsigned char *p, const unsigned char *q)
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_core_ed25519_sub(unsigned char *r,
                            const unsigned char *p, const unsigned char *q)
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_core_ed25519_from_uniform(unsigned char *p, const unsigned char *r)
            __attribute__ ((nonnull));

SODIUM_EXPORT
void crypto_core_ed25519_scalar_random(unsigned char *r)
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_core_ed25519_scalar_invert(unsigned char *recip, const unsigned char s[64])
            __attribute__ ((nonnull));

/*
 * The interval `s` is sampled from should be at least 317 bits to ensure almost
 * uniformity of `r` over `L`.
 */
SODIUM_EXPORT
void crypto_core_ed25519_scalar_reduce(unsigned char *r, const unsigned char s[64])
            __attribute__ ((nonnull));

#ifdef __cplusplus
}
#endif

#endif
