#ifndef crypto_kem_H
#define crypto_kem_H

/*
 * THREAD SAFETY: crypto_kem_keypair() is thread-safe,
 * provided that sodium_init() was called before.
 *
 * Other functions are always thread-safe.
 */

#include <stddef.h>

#include "crypto_kem_xwing.h"
#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_kem_PUBLICKEYBYTES crypto_kem_xwing_PUBLICKEYBYTES
SODIUM_EXPORT
size_t crypto_kem_publickeybytes(void);

#define crypto_kem_SECRETKEYBYTES crypto_kem_xwing_SECRETKEYBYTES
SODIUM_EXPORT
size_t crypto_kem_secretkeybytes(void);

#define crypto_kem_CIPHERTEXTBYTES crypto_kem_xwing_CIPHERTEXTBYTES
SODIUM_EXPORT
size_t crypto_kem_ciphertextbytes(void);

#define crypto_kem_SHAREDSECRETBYTES crypto_kem_xwing_SHAREDSECRETBYTES
SODIUM_EXPORT
size_t crypto_kem_sharedsecretbytes(void);

#define crypto_kem_SEEDBYTES crypto_kem_xwing_SEEDBYTES
SODIUM_EXPORT
size_t crypto_kem_seedbytes(void);

#define crypto_kem_PRIMITIVE "xwing"
SODIUM_EXPORT
const char *crypto_kem_primitive(void);

SODIUM_EXPORT
int crypto_kem_seed_keypair(unsigned char *pk, unsigned char *sk,
                            const unsigned char *seed)
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_kem_enc(unsigned char *ct, unsigned char *ss,
                   const unsigned char *pk)
            __attribute__ ((warn_unused_result)) __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_kem_dec(unsigned char *ss, const unsigned char *ct,
                   const unsigned char *sk)
            __attribute__ ((warn_unused_result)) __attribute__ ((nonnull));

#ifdef __cplusplus
}
#endif

#endif
