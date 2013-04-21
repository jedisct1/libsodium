#ifndef crypto_auth_H
#define crypto_auth_H

#include <stdlib.h>

#include "crypto_auth_hmacsha512256.h"

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_auth_BYTES crypto_auth_hmacsha512256_BYTES
size_t  crypto_auth_bytes(void);

#define crypto_auth_KEYBYTES crypto_auth_hmacsha512256_KEYBYTES
size_t  crypto_auth_keybytes(void);

#define crypto_auth_PRIMITIVE "hmacsha512256"
const char *crypto_auth_primitive(void);

int crypto_auth(unsigned char *out, const unsigned char *in,
                unsigned long long inlen, const unsigned char *k);

int crypto_auth_verify(const unsigned char *h, const unsigned char *in,
                       unsigned long long inlen,const unsigned char *k);
#ifdef __cplusplus
}
#endif

#endif
