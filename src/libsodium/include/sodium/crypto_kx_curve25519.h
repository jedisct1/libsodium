#ifndef crypto_kx_curve25519_H
#define crypto_kx_curve25519_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_kx_curve25519_PUBLICKEYBYTES 32
SODIUM_EXPORT
size_t crypto_kx_curve25519_publickeybytes(void);

#define crypto_kx_curve25519_SECRETKEYBYTES 32
SODIUM_EXPORT
size_t crypto_kx_curve25519_secretkeybytes(void);

#define crypto_kx_curve25519_SEEDBYTES 32
SODIUM_EXPORT
size_t crypto_kx_curve25519_seedbytes(void);

#define crypto_kx_curve25519_SESSIONKEYBYTES 32
SODIUM_EXPORT
size_t crypto_kx_curve25519_sessionkeybytes(void);

#define crypto_kx_curve25519_PRIMITIVE "x25519blake2b"
SODIUM_EXPORT
const char *crypto_kx_curve25519_primitive(void);

SODIUM_EXPORT
int crypto_kx_curve25519_seed_keypair(unsigned char pk[crypto_kx_curve25519_PUBLICKEYBYTES],
                                      unsigned char sk[crypto_kx_curve25519_SECRETKEYBYTES],
                                      const unsigned char seed[crypto_kx_curve25519_SEEDBYTES])
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_kx_curve25519_keypair(unsigned char pk[crypto_kx_curve25519_PUBLICKEYBYTES],
                                 unsigned char sk[crypto_kx_curve25519_SECRETKEYBYTES])
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_kx_curve25519_client_session_keys(unsigned char rx[crypto_kx_curve25519_SESSIONKEYBYTES],
                                             unsigned char tx[crypto_kx_curve25519_SESSIONKEYBYTES],
                                             const unsigned char client_pk[crypto_kx_curve25519_PUBLICKEYBYTES],
                                             const unsigned char client_sk[crypto_kx_curve25519_SECRETKEYBYTES],
                                             const unsigned char server_pk[crypto_kx_curve25519_PUBLICKEYBYTES])
            __attribute__ ((warn_unused_result))  __attribute__ ((nonnull(3, 4, 5)));

SODIUM_EXPORT
int crypto_kx_curve25519_server_session_keys(unsigned char rx[crypto_kx_curve25519_SESSIONKEYBYTES],
                                             unsigned char tx[crypto_kx_curve25519_SESSIONKEYBYTES],
                                             const unsigned char server_pk[crypto_kx_curve25519_PUBLICKEYBYTES],
                                             const unsigned char server_sk[crypto_kx_curve25519_SECRETKEYBYTES],
                                             const unsigned char client_pk[crypto_kx_curve25519_PUBLICKEYBYTES])
            __attribute__ ((warn_unused_result))  __attribute__ ((nonnull(3, 4, 5)));

#ifdef __cplusplus
}
#endif

#endif
