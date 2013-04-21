#ifndef crypto_secretbox_xsalsa20poly1305_H
#define crypto_secretbox_xsalsa20poly1305_H

#include "export.h"

#define crypto_secretbox_xsalsa20poly1305_KEYBYTES 32
#define crypto_secretbox_xsalsa20poly1305_NONCEBYTES 24
#define crypto_secretbox_xsalsa20poly1305_ZEROBYTES 32
#define crypto_secretbox_xsalsa20poly1305_BOXZEROBYTES 16

#ifdef __cplusplus
extern "C" {
#endif

int crypto_secretbox_xsalsa20poly1305_ref(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
int crypto_secretbox_xsalsa20poly1305_ref_open(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_secretbox_xsalsa20poly1305 crypto_secretbox_xsalsa20poly1305_ref
#define crypto_secretbox_xsalsa20poly1305_open crypto_secretbox_xsalsa20poly1305_ref_open

#endif
