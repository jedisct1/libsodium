#ifndef crypto_box_curve25519xsalsa20poly1305_H
#define crypto_box_curve25519xsalsa20poly1305_H

#include <stddef.h>
#include "export.h"

#define crypto_box_curve25519xsalsa20poly1305_PUBLICKEYBYTES 32U
#define crypto_box_curve25519xsalsa20poly1305_SECRETKEYBYTES 32U
#define crypto_box_curve25519xsalsa20poly1305_BEFORENMBYTES 32U
#define crypto_box_curve25519xsalsa20poly1305_NONCEBYTES 24U
#define crypto_box_curve25519xsalsa20poly1305_ZEROBYTES 32U
#define crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES 16U
#define crypto_box_curve25519xsalsa20poly1305_MACBYTES (crypto_box_curve25519xsalsa20poly1305_ZEROBYTES - crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES)

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_box_curve25519xsalsa20poly1305_publickeybytes(void);

SODIUM_EXPORT
size_t crypto_box_curve25519xsalsa20poly1305_secretkeybytes(void);

SODIUM_EXPORT
size_t crypto_box_curve25519xsalsa20poly1305_beforenmbytes(void);

SODIUM_EXPORT
size_t crypto_box_curve25519xsalsa20poly1305_noncebytes(void);

SODIUM_EXPORT
size_t crypto_box_curve25519xsalsa20poly1305_zerobytes(void);

SODIUM_EXPORT
size_t crypto_box_curve25519xsalsa20poly1305_boxzerobytes(void);

SODIUM_EXPORT
size_t crypto_box_curve25519xsalsa20poly1305_macbytes(void);

SODIUM_EXPORT
const char * crypto_box_curve25519xsalsa20poly1305_primitive(void);

SODIUM_EXPORT
int crypto_box_curve25519xsalsa20poly1305(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_box_curve25519xsalsa20poly1305_open(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_box_curve25519xsalsa20poly1305_keypair(unsigned char *,unsigned char *);

SODIUM_EXPORT
int crypto_box_curve25519xsalsa20poly1305_beforenm(unsigned char *,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_box_curve25519xsalsa20poly1305_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_box_curve25519xsalsa20poly1305_open_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_box_curve25519xsalsa20poly1305_ref crypto_box_curve25519xsalsa20poly1305
#define crypto_box_curve25519xsalsa20poly1305_ref_open crypto_box_curve25519xsalsa20poly1305_open
#define crypto_box_curve25519xsalsa20poly1305_ref_keypair crypto_box_curve25519xsalsa20poly1305_keypair
#define crypto_box_curve25519xsalsa20poly1305_ref_beforenm crypto_box_curve25519xsalsa20poly1305_beforenm
#define crypto_box_curve25519xsalsa20poly1305_ref_afternm crypto_box_curve25519xsalsa20poly1305_afternm
#define crypto_box_curve25519xsalsa20poly1305_ref_open_afternm crypto_box_curve25519xsalsa20poly1305_open_afternm

#endif
