#ifndef crypto_sign_mldsa44_H
#define crypto_sign_mldsa44_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
#    ifdef __GNUC__
#        pragma GCC diagnostic ignored "-Wlong-long"
#    endif
extern "C" {
#endif

#define crypto_sign_mldsa44_BYTES 2420U
SODIUM_EXPORT
size_t crypto_sign_mldsa44_bytes(void);

#define crypto_sign_mldsa44_PUBLICKEYBYTES 1312U
SODIUM_EXPORT
size_t crypto_sign_mldsa44_publickeybytes(void);

#define crypto_sign_mldsa44_SECRETKEYBYTES 2560U
SODIUM_EXPORT
size_t crypto_sign_mldsa44_secretkeybytes(void);

#define crypto_sign_mldsa44_SEEDBYTES 32U
SODIUM_EXPORT
size_t crypto_sign_mldsa44_seedbytes(void);

#define crypto_sign_mldsa44_CONTEXTBYTES_MAX 255U
SODIUM_EXPORT
size_t crypto_sign_mldsa44_contextbytes_max(void);

#define crypto_sign_mldsa44_MESSAGEBYTES_MAX (SODIUM_SIZE_MAX - crypto_sign_mldsa44_BYTES)
SODIUM_EXPORT
size_t crypto_sign_mldsa44_messagebytes_max(void);

SODIUM_EXPORT
int crypto_sign_mldsa44_seed_keypair(unsigned char       pk[crypto_sign_mldsa44_PUBLICKEYBYTES],
                                     unsigned char       sk[crypto_sign_mldsa44_SECRETKEYBYTES],
                                     const unsigned char seed[crypto_sign_mldsa44_SEEDBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
int crypto_sign_mldsa44_keypair(unsigned char pk[crypto_sign_mldsa44_PUBLICKEYBYTES],
                                unsigned char sk[crypto_sign_mldsa44_SECRETKEYBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
int crypto_sign_mldsa44_detached(unsigned char       sig[crypto_sign_mldsa44_BYTES],
                                 unsigned long long *siglen_p,
                                 const unsigned char *m,
                                 unsigned long long   mlen,
                                 const unsigned char sk[crypto_sign_mldsa44_SECRETKEYBYTES])
    __attribute__((nonnull(1, 5)));

SODIUM_EXPORT
int crypto_sign_mldsa44_detached_ctx(unsigned char       sig[crypto_sign_mldsa44_BYTES],
                                     unsigned long long *siglen_p,
                                     const unsigned char *m,
                                     unsigned long long   mlen,
                                     const unsigned char *ctx,
                                     size_t               ctxlen,
                                     const unsigned char sk[crypto_sign_mldsa44_SECRETKEYBYTES])
    __attribute__((nonnull(1, 7)));

SODIUM_EXPORT
int crypto_sign_mldsa44_verify_detached(const unsigned char sig[crypto_sign_mldsa44_BYTES],
                                        const unsigned char *m,
                                        unsigned long long   mlen,
                                        const unsigned char pk[crypto_sign_mldsa44_PUBLICKEYBYTES])
    __attribute__((warn_unused_result)) __attribute__((nonnull(1, 4)));

SODIUM_EXPORT
int crypto_sign_mldsa44_verify_detached_ctx(
    const unsigned char sig[crypto_sign_mldsa44_BYTES],
    const unsigned char *m,
    unsigned long long   mlen,
    const unsigned char *ctx,
    size_t               ctxlen,
    const unsigned char pk[crypto_sign_mldsa44_PUBLICKEYBYTES])
    __attribute__((warn_unused_result)) __attribute__((nonnull(1, 6)));

#ifdef __cplusplus
}
#endif

#endif
