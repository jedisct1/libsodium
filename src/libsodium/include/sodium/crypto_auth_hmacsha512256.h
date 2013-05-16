#ifndef crypto_auth_hmacsha512256_H
#define crypto_auth_hmacsha512256_H

#include <stddef.h>
#include "export.h"

#define crypto_auth_hmacsha512256_BYTES 32U
#define crypto_auth_hmacsha512256_KEYBYTES 32U

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_auth_hmacsha512256_bytes(void);

SODIUM_EXPORT
size_t crypto_auth_hmacsha512256_keybytes(void);

SODIUM_EXPORT
const char * crypto_auth_hmacsha512256_primitive(void);

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
