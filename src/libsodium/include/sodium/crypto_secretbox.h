#ifndef crypto_secretbox_H
#define crypto_secretbox_H

#include <stdlib.h>

#include "crypto_secretbox_xsalsa20poly1305.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_secretbox_KEYBYTES crypto_secretbox_xsalsa20poly1305_KEYBYTES
size_t  crypto_secretbox_keybytes(void);

#define crypto_secretbox_NONCEBYTES crypto_secretbox_xsalsa20poly1305_NONCEBYTES
size_t  crypto_secretbox_noncebytes(void);

#define crypto_secretbox_ZEROBYTES crypto_secretbox_xsalsa20poly1305_ZEROBYTES
size_t  crypto_secretbox_zerobytes(void);

#define crypto_secretbox_BOXZEROBYTES crypto_secretbox_xsalsa20poly1305_BOXZEROBYTES
size_t  crypto_secretbox_boxzerobytes(void);

#define crypto_secretbox_PRIMITIVE "xsalsa20poly1305"
const char *crypto_secretbox_primitive(void);

int crypto_secretbox(unsigned char *c, const unsigned char *m,
                     unsigned long long mlen, const unsigned char *n,
                     const unsigned char *k);

int crypto_secretbox_open(unsigned char *m, const unsigned char *c,
                          unsigned long long clen, const unsigned char *n,
                          const unsigned char *k);

#ifdef __cplusplus
}
#endif

#endif
