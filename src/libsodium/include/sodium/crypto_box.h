#ifndef crypto_box_H
#define crypto_box_H

#include "crypto_box_curve25519xsalsa20poly1305.h"

#define crypto_box_PUBLICKEYBYTES crypto_box_curve25519xsalsa20poly1305_PUBLICKEYBYTES
#define crypto_box_SECRETKEYBYTES crypto_box_curve25519xsalsa20poly1305_SECRETKEYBYTES
#define crypto_box_BEFORENMBYTES crypto_box_curve25519xsalsa20poly1305_BEFORENMBYTES
#define crypto_box_NONCEBYTES crypto_box_curve25519xsalsa20poly1305_NONCEBYTES
#define crypto_box_ZEROBYTES crypto_box_curve25519xsalsa20poly1305_ZEROBYTES
#define crypto_box_BOXZEROBYTES crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES
#define crypto_box_MACBYTES crypto_box_curve25519xsalsa20poly1305_MACBYTES
#define crypto_box_PRIMITIVE "curve25519xsalsa20poly1305"

int crypto_box_keypair(unsigned char *pk, unsigned char *sk);

int crypto_box_beforenm(unsigned char *k, const unsigned char *pk,
                        const unsigned char *sk);

int crypto_box_afternm(unsigned char *c, const unsigned char *m,
                       unsigned long long mlen, const unsigned char *n,
                       const unsigned char *k);

int crypto_box_open_afternm(unsigned char *m, const unsigned char *c,
                            unsigned long long clen, const unsigned char *n,
                            const unsigned char *k);

int crypto_box(unsigned char *c, const unsigned char *m,
               unsigned long long mlen, const unsigned char *n,
               const unsigned char *pk, const unsigned char *sk);

int crypto_box_open(unsigned char *m, const unsigned char *c,
                    unsigned long long clen, const unsigned char *n,
                    const unsigned char *pk, const unsigned char *sk);

#endif
