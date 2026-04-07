#ifndef crypto_kem_mlkem768_H
#define crypto_kem_mlkem768_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
#    ifdef __GNUC__
#        pragma GCC diagnostic ignored "-Wlong-long"
#    endif
extern "C" {
#endif

#define crypto_kem_mlkem768_PUBLICKEYBYTES 1184U
SODIUM_EXPORT
size_t crypto_kem_mlkem768_publickeybytes(void);

#define crypto_kem_mlkem768_SECRETKEYBYTES 2400U
SODIUM_EXPORT
size_t crypto_kem_mlkem768_secretkeybytes(void);

#define crypto_kem_mlkem768_CIPHERTEXTBYTES 1088U
SODIUM_EXPORT
size_t crypto_kem_mlkem768_ciphertextbytes(void);

#define crypto_kem_mlkem768_SHAREDSECRETBYTES 32U
SODIUM_EXPORT
size_t crypto_kem_mlkem768_sharedsecretbytes(void);

#define crypto_kem_mlkem768_SEEDBYTES 64U
SODIUM_EXPORT
size_t crypto_kem_mlkem768_seedbytes(void);

SODIUM_EXPORT
int crypto_kem_mlkem768_seed_keypair(unsigned char       pk[crypto_kem_mlkem768_PUBLICKEYBYTES],
                                     unsigned char       sk[crypto_kem_mlkem768_SECRETKEYBYTES],
                                     const unsigned char seed[crypto_kem_mlkem768_SEEDBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
int crypto_kem_mlkem768_keypair(unsigned char pk[crypto_kem_mlkem768_PUBLICKEYBYTES],
                                unsigned char sk[crypto_kem_mlkem768_SECRETKEYBYTES])
    __attribute__((nonnull));

SODIUM_EXPORT
int crypto_kem_mlkem768_enc(unsigned char       ct[crypto_kem_mlkem768_CIPHERTEXTBYTES],
                            unsigned char       ss[crypto_kem_mlkem768_SHAREDSECRETBYTES],
                            const unsigned char pk[crypto_kem_mlkem768_PUBLICKEYBYTES])
    __attribute__((warn_unused_result)) __attribute__((nonnull));

SODIUM_EXPORT
int
crypto_kem_mlkem768_enc_deterministic(unsigned char       ct[crypto_kem_mlkem768_CIPHERTEXTBYTES],
                                      unsigned char       ss[crypto_kem_mlkem768_SHAREDSECRETBYTES],
                                      const unsigned char pk[crypto_kem_mlkem768_PUBLICKEYBYTES],
                                      const unsigned char seed[32])
    __attribute__((warn_unused_result)) __attribute__((nonnull));

SODIUM_EXPORT
int crypto_kem_mlkem768_dec(unsigned char       ss[crypto_kem_mlkem768_SHAREDSECRETBYTES],
                            const unsigned char ct[crypto_kem_mlkem768_CIPHERTEXTBYTES],
                            const unsigned char sk[crypto_kem_mlkem768_SECRETKEYBYTES])
    __attribute__((warn_unused_result)) __attribute__((nonnull));

#ifdef __cplusplus
}
#endif

#endif
