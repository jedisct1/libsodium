
#include "crypto_kx.h"
#include "crypto_kx_curve25519.h"
#include "private/common.h"

int
crypto_kx_seed_keypair(unsigned char pk[crypto_kx_PUBLICKEYBYTES],
                       unsigned char sk[crypto_kx_SECRETKEYBYTES],
                       const unsigned char seed[crypto_kx_SEEDBYTES])
{
    return crypto_kx_curve25519_seed_keypair(pk, sk, seed);
}

int
crypto_kx_keypair(unsigned char pk[crypto_kx_PUBLICKEYBYTES],
                  unsigned char sk[crypto_kx_SECRETKEYBYTES])
{
    return crypto_kx_curve25519_keypair(pk, sk);
}

int
crypto_kx_client_session_keys(unsigned char rx[crypto_kx_SESSIONKEYBYTES],
                              unsigned char tx[crypto_kx_SESSIONKEYBYTES],
                              const unsigned char client_pk[crypto_kx_PUBLICKEYBYTES],
                              const unsigned char client_sk[crypto_kx_SECRETKEYBYTES],
                              const unsigned char server_pk[crypto_kx_PUBLICKEYBYTES])
{
    return crypto_kx_curve25519_client_session_keys(rx, tx, client_pk,
                                                    client_sk, server_pk);
}

int
crypto_kx_server_session_keys(unsigned char rx[crypto_kx_SESSIONKEYBYTES],
                              unsigned char tx[crypto_kx_SESSIONKEYBYTES],
                              const unsigned char server_pk[crypto_kx_PUBLICKEYBYTES],
                              const unsigned char server_sk[crypto_kx_SECRETKEYBYTES],
                              const unsigned char client_pk[crypto_kx_PUBLICKEYBYTES])
{
    return crypto_kx_curve25519_server_session_keys(rx, tx, server_pk,
                                                    server_sk, client_pk);
}

size_t
crypto_kx_publickeybytes(void)
{
    return crypto_kx_curve25519_PUBLICKEYBYTES;
}

size_t
crypto_kx_secretkeybytes(void)
{
    return crypto_kx_curve25519_SECRETKEYBYTES;
}

size_t
crypto_kx_seedbytes(void)
{
    return crypto_kx_curve25519_SEEDBYTES;
}

size_t
crypto_kx_sessionkeybytes(void)
{
    return crypto_kx_curve25519_SESSIONKEYBYTES;
}

const char *
crypto_kx_primitive(void)
{
    return crypto_kx_curve25519_PRIMITIVE;
}
