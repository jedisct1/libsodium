#ifndef crypto_sign_schnorr_H
#define crypto_sign_schnorr_H

#include <stddef.h>

#define crypto_sign_schnorr_PUBLICKEYBYTES 33
#define crypto_sign_schnorr_SECRETKEYBYTES 32
#define crypto_sign_schnorr_BYTES 65

int crypto_sign_schnorr_keypair(unsigned char *pk, unsigned char *sk);
int crypto_sign_schnorr(unsigned char *sm, unsigned long long *smlen,
                         const unsigned char *m, unsigned long long mlen,
                         const unsigned char *sk);
int crypto_sign_schnorr_open(unsigned char *m, unsigned long long *mlen,
                              const unsigned char *sm, unsigned long long smlen,
                              const unsigned char *pk);

#endif
