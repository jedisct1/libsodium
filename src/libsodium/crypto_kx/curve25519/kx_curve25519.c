
#include <stddef.h>

#include "core.h"
#include "crypto_generichash.h"
#include "crypto_kx_curve25519.h"
#include "crypto_scalarmult_curve25519.h"
#include "private/common.h"
#include "randombytes.h"
#include "utils.h"

int
crypto_kx_curve25519_seed_keypair(unsigned char pk[crypto_kx_curve25519_PUBLICKEYBYTES],
                                  unsigned char sk[crypto_kx_curve25519_SECRETKEYBYTES],
                                  const unsigned char seed[crypto_kx_curve25519_SEEDBYTES])
{
    crypto_generichash(sk, crypto_kx_curve25519_SECRETKEYBYTES,
                       seed, crypto_kx_curve25519_SEEDBYTES, NULL, 0);
    return crypto_scalarmult_curve25519_base(pk, sk);
}

int
crypto_kx_curve25519_keypair(unsigned char pk[crypto_kx_curve25519_PUBLICKEYBYTES],
                             unsigned char sk[crypto_kx_curve25519_SECRETKEYBYTES])
{
    COMPILER_ASSERT(crypto_kx_curve25519_SECRETKEYBYTES == crypto_scalarmult_curve25519_SCALARBYTES);
    COMPILER_ASSERT(crypto_kx_curve25519_PUBLICKEYBYTES == crypto_scalarmult_curve25519_BYTES);

    randombytes_buf(sk, crypto_kx_curve25519_SECRETKEYBYTES);
    return crypto_scalarmult_curve25519_base(pk, sk);
}

int
crypto_kx_curve25519_client_session_keys(unsigned char rx[crypto_kx_curve25519_SESSIONKEYBYTES],
                                         unsigned char tx[crypto_kx_curve25519_SESSIONKEYBYTES],
                                         const unsigned char client_pk[crypto_kx_curve25519_PUBLICKEYBYTES],
                                         const unsigned char client_sk[crypto_kx_curve25519_SECRETKEYBYTES],
                                         const unsigned char server_pk[crypto_kx_curve25519_PUBLICKEYBYTES])
{
    crypto_generichash_state h;
    unsigned char            q[crypto_scalarmult_curve25519_BYTES];
    unsigned char            keys[2 * crypto_kx_curve25519_SESSIONKEYBYTES];
    int                      i;

    if (rx == NULL) {
        rx = tx;
    }
    if (tx == NULL) {
        tx = rx;
    }
    if (rx == NULL) {
        sodium_misuse(); /* LCOV_EXCL_LINE */
    }
    if (crypto_scalarmult_curve25519(q, client_sk, server_pk) != 0) {
        return -1;
    }
    COMPILER_ASSERT(sizeof keys <= crypto_generichash_BYTES_MAX);
    crypto_generichash_init(&h, NULL, 0U, sizeof keys);
    crypto_generichash_update(&h, q, crypto_scalarmult_curve25519_BYTES);
    sodium_memzero(q, sizeof q);
    crypto_generichash_update(&h, client_pk, crypto_kx_curve25519_PUBLICKEYBYTES);
    crypto_generichash_update(&h, server_pk, crypto_kx_curve25519_PUBLICKEYBYTES);
    crypto_generichash_final(&h, keys, sizeof keys);
    sodium_memzero(&h, sizeof h);
    for (i = 0; i < crypto_kx_curve25519_SESSIONKEYBYTES; i++) {
        rx[i] = keys[i];
        tx[i] = keys[i + crypto_kx_curve25519_SESSIONKEYBYTES];
    }
    sodium_memzero(keys, sizeof keys);

    return 0;
}

int
crypto_kx_curve25519_server_session_keys(unsigned char rx[crypto_kx_curve25519_SESSIONKEYBYTES],
                                         unsigned char tx[crypto_kx_curve25519_SESSIONKEYBYTES],
                                         const unsigned char server_pk[crypto_kx_curve25519_PUBLICKEYBYTES],
                                         const unsigned char server_sk[crypto_kx_curve25519_SECRETKEYBYTES],
                                         const unsigned char client_pk[crypto_kx_curve25519_PUBLICKEYBYTES])
{
    crypto_generichash_state h;
    unsigned char            q[crypto_scalarmult_curve25519_BYTES];
    unsigned char            keys[2 * crypto_kx_curve25519_SESSIONKEYBYTES];
    int                      i;

    if (rx == NULL) {
        rx = tx;
    }
    if (tx == NULL) {
        tx = rx;
    }
    if (rx == NULL) {
        sodium_misuse(); /* LCOV_EXCL_LINE */
    }
    if (crypto_scalarmult_curve25519(q, server_sk, client_pk) != 0) {
        return -1;
    }
    COMPILER_ASSERT(sizeof keys <= crypto_generichash_BYTES_MAX);
    crypto_generichash_init(&h, NULL, 0U, sizeof keys);
    crypto_generichash_update(&h, q, crypto_scalarmult_curve25519_BYTES);
    sodium_memzero(q, sizeof q);
    crypto_generichash_update(&h, client_pk, crypto_kx_curve25519_PUBLICKEYBYTES);
    crypto_generichash_update(&h, server_pk, crypto_kx_curve25519_PUBLICKEYBYTES);
    crypto_generichash_final(&h, keys, sizeof keys);
    sodium_memzero(&h, sizeof h);
    for (i = 0; i < crypto_kx_curve25519_SESSIONKEYBYTES; i++) {
        tx[i] = keys[i];
        rx[i] = keys[i + crypto_kx_curve25519_SESSIONKEYBYTES];
    }
    sodium_memzero(keys, sizeof keys);

    return 0;
}

size_t
crypto_kx_curve25519_publickeybytes(void)
{
    return crypto_kx_curve25519_PUBLICKEYBYTES;
}

size_t
crypto_kx_curve25519_secretkeybytes(void)
{
    return crypto_kx_curve25519_SECRETKEYBYTES;
}

size_t
crypto_kx_curve25519_seedbytes(void)
{
    return crypto_kx_curve25519_SEEDBYTES;
}

size_t
crypto_kx_curve25519_sessionkeybytes(void)
{
    return crypto_kx_curve25519_SESSIONKEYBYTES;
}

const char *
crypto_kx_curve25519_primitive(void)
{
    return crypto_kx_curve25519_PRIMITIVE;
}
