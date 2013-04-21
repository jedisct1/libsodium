#ifndef crypto_box_H
#define crypto_box_H

#include <stdlib.h>

#include "crypto_box_curve25519xsalsa20poly1305.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_box_PUBLICKEYBYTES crypto_box_curve25519xsalsa20poly1305_PUBLICKEYBYTES
size_t  crypto_box_publickeybytes(void);

#define crypto_box_SECRETKEYBYTES crypto_box_curve25519xsalsa20poly1305_SECRETKEYBYTES
size_t  crypto_box_secretkeybytes(void);

#define crypto_box_BEFORENMBYTES crypto_box_curve25519xsalsa20poly1305_BEFORENMBYTES
size_t  crypto_box_beforenmbytes(void);

#define crypto_box_NONCEBYTES crypto_box_curve25519xsalsa20poly1305_NONCEBYTES
size_t  crypto_box_noncebytes(void);

#define crypto_box_ZEROBYTES crypto_box_curve25519xsalsa20poly1305_ZEROBYTES
size_t  crypto_box_zerobytes(void);

#define crypto_box_BOXZEROBYTES crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES
size_t  crypto_box_boxzerobytes(void);

#define crypto_box_MACBYTES crypto_box_curve25519xsalsa20poly1305_MACBYTES
size_t  crypto_box_macbytes(void);

#define crypto_box_PRIMITIVE "curve25519xsalsa20poly1305"
const char *crypto_box_primitive(void);

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

#ifdef __cplusplus
}
#endif

#endif
