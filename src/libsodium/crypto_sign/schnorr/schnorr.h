#ifndef SCHNORR_H
#define SCHNORR_H

#include <stddef.h>

#define crypto_sign_schnorr_PUBLICKEYBYTES 33
#define crypto_sign_schnorr_SECRETKEYBYTES 32
#define crypto_sign_schnorr_BYTES 65

int crypto_sign_schnorr_keypair(unsigned char *pk, unsigned char *sk);
int crypto_sign_schnorr(unsigned char *sig, unsigned long long *siglen,
                         const unsigned char *msg, unsigned long long msglen,
                         const unsigned char *sk);
int crypto_sign_schnorr_verify(const unsigned char *msg, unsigned long long msglen,
                                const unsigned char *sig, unsigned long long siglen,
                                const unsigned char *pk);

#endif
