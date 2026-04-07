#ifndef crypto_kem_xwing_H
#define crypto_kem_xwing_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
#    ifdef __GNUC__
#        pragma GCC diagnostic ignored "-Wlong-long"
#    endif
extern "C" {
#endif

#define crypto_kem_xwing_PUBLICKEYBYTES 1216U
SODIUM_EXPORT
size_t crypto_kem_xwing_publickeybytes(void);

#define crypto_kem_xwing_SECRETKEYBYTES 32U
SODIUM_EXPORT
size_t crypto_kem_xwing_secretkeybytes(void);

#define crypto_kem_xwing_CIPHERTEXTBYTES 1120U
SODIUM_EXPORT
size_t crypto_kem_xwing_ciphertextbytes(void);

#define crypto_kem_xwing_SHAREDSECRETBYTES 32U
SODIUM_EXPORT
size_t crypto_kem_xwing_sharedsecretbytes(void);

#define crypto_kem_xwing_SEEDBYTES 32U
SODIUM_EXPORT
size_t crypto_kem_xwing_seedbytes(void);

SODIUM_EXPORT
int crypto_kem_xwing_seed_keypair(unsigned char       pk[crypto_kem_xwing_PUBLICKEYBYTES],
                                  unsigned char       sk[crypto_kem_xwing_SECRETKEYBYTES],
                                  const unsigned char seed[crypto_kem_xwing_SEEDBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
int crypto_kem_xwing_keypair(unsigned char pk[crypto_kem_xwing_PUBLICKEYBYTES],
                             unsigned char sk[crypto_kem_xwing_SECRETKEYBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
int crypto_kem_xwing_enc(unsigned char       ct[crypto_kem_xwing_CIPHERTEXTBYTES],
                         unsigned char       ss[crypto_kem_xwing_SHAREDSECRETBYTES],
                         const unsigned char pk[crypto_kem_xwing_PUBLICKEYBYTES])
    __attribute__((warn_unused_result)) __attribute__((nonnull));

SODIUM_EXPORT
int crypto_kem_xwing_enc_deterministic(unsigned char       ct[crypto_kem_xwing_CIPHERTEXTBYTES],
                                       unsigned char       ss[crypto_kem_xwing_SHAREDSECRETBYTES],
                                       const unsigned char pk[crypto_kem_xwing_PUBLICKEYBYTES],
                                       const unsigned char seed[64])
    __attribute__((warn_unused_result)) __attribute__((nonnull));

SODIUM_EXPORT
int crypto_kem_xwing_dec(unsigned char       ss[crypto_kem_xwing_SHAREDSECRETBYTES],
                         const unsigned char ct[crypto_kem_xwing_CIPHERTEXTBYTES],
                         const unsigned char sk[crypto_kem_xwing_SECRETKEYBYTES])
    __attribute__((warn_unused_result)) __attribute__((nonnull));

#ifdef __cplusplus
}
#endif

#endif
