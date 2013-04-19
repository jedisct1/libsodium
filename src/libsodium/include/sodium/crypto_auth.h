#ifndef crypto_auth_H
#define crypto_auth_H

#include "crypto_auth_hmacsha512256.h"

#define crypto_auth_BYTES crypto_auth_hmacsha512256_BYTES
#define crypto_auth_KEYBYTES crypto_auth_hmacsha512256_KEYBYTES
#define crypto_auth_PRIMITIVE "hmacsha512256"

#ifdef __cplusplus
extern "C" {
#endif
int crypto_auth(unsigned char *out, const unsigned char *in,
                unsigned long long inlen, const unsigned char *k);

int crypto_auth_verify(const unsigned char *h, const unsigned char *in,
                       unsigned long long inlen,const unsigned char *k);
#ifdef __cplusplus
}
#endif

#endif
