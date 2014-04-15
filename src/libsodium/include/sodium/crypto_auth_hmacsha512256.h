#ifndef crypto_auth_hmacsha512256_H
#define crypto_auth_hmacsha512256_H

#include <stddef.h>
#include "crypto_auth_hmacsha512.h"
#include "export.h"

#define crypto_auth_hmacsha512256_BYTES 32U
#define crypto_auth_hmacsha512256_KEYBYTES 32U

#ifdef __cplusplus
# if __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

typedef struct crypto_auth_hmacsha512_state crypto_auth_hmacsha512256_state;

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

SODIUM_EXPORT
int crypto_auth_hmacsha512256_init(crypto_auth_hmacsha512256_state *state,
                                   const unsigned char *key,
                                   size_t keylen);

SODIUM_EXPORT
int crypto_auth_hmacsha512256_update(crypto_auth_hmacsha512256_state *state,
                                     const unsigned char *in,
                                     unsigned long long inlen);

SODIUM_EXPORT
int crypto_auth_hmacsha512256_final(crypto_auth_hmacsha512256_state *state,
                                    unsigned char *out);

#ifdef __cplusplus
}
#endif

#endif
