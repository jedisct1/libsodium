#ifndef crypto_sign_H
#define crypto_sign_H

#include "crypto_sign_ed25519.h"

#define crypto_sign_BYTES crypto_sign_ed25519_BYTES
#define crypto_sign_PUBLICKEYBYTES crypto_sign_ed25519_PUBLICKEYBYTES
#define crypto_sign_SECRETKEYBYTES crypto_sign_ed25519_SECRETKEYBYTES
#define crypto_sign_PRIMITIVE "ed25519"

#ifdef __cplusplus
extern "C" {
#endif

int crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk,
                             const unsigned char *seed);

int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);

int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk);

int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk);

#ifdef __cplusplus
}
#endif
    
#endif
