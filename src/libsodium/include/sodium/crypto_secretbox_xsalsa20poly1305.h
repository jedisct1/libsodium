#ifndef crypto_secretbox_xsalsa20poly1305_H
#define crypto_secretbox_xsalsa20poly1305_H

#include <stddef.h>
#include <stdint.h>
#include "export.h"

#define crypto_secretbox_xsalsa20poly1305_KEYBYTES 32U
#define crypto_secretbox_xsalsa20poly1305_NONCEBYTES 24U
#define crypto_secretbox_xsalsa20poly1305_ZEROBYTES 32U
#define crypto_secretbox_xsalsa20poly1305_BOXZEROBYTES 16U
#define crypto_secretbox_xsalsa20poly1305_MACBYTES (crypto_secretbox_xsalsa20poly1305_ZEROBYTES - crypto_secretbox_xsalsa20poly1305_BOXZEROBYTES)

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_secretbox_xsalsa20poly1305_keybytes(void);

SODIUM_EXPORT
size_t crypto_secretbox_xsalsa20poly1305_noncebytes(void);

SODIUM_EXPORT
size_t crypto_secretbox_xsalsa20poly1305_zerobytes(void);

SODIUM_EXPORT
size_t crypto_secretbox_xsalsa20poly1305_boxzerobytes(void);

SODIUM_EXPORT
size_t crypto_secretbox_xsalsa20poly1305_macbytes(void);

SODIUM_EXPORT
const char * crypto_secretbox_xsalsa20poly1305_primitive(void);

SODIUM_EXPORT
int crypto_secretbox_xsalsa20poly1305(unsigned char *,const unsigned char *,uint64_t,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_secretbox_xsalsa20poly1305_open(unsigned char *,const unsigned char *,uint64_t,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_secretbox_xsalsa20poly1305_ref crypto_secretbox_xsalsa20poly1305
#define crypto_secretbox_xsalsa20poly1305_ref_open crypto_secretbox_xsalsa20poly1305_open

#endif
