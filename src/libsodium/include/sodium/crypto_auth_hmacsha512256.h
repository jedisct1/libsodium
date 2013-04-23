#ifndef crypto_auth_hmacsha512256_H
#define crypto_auth_hmacsha512256_H

#include "export.h"

#define crypto_auth_hmacsha512256_BYTES 32
#define crypto_auth_hmacsha512256_KEYBYTES 32

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
int crypto_auth_hmacsha512256(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);

SODIUM_EXPORT
int crypto_auth_hmacsha512256_verify(const unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_auth_hmacsha512256_ref crypto_auth_hmacsha512256
#define crypto_auth_hmacsha512256_ref_verify crypto_auth_hmacsha512256_verify

#endif
