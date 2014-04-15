#ifndef crypto_auth_hmacsha256_H
#define crypto_auth_hmacsha256_H

#include <stddef.h>
#include "crypto_hash_sha256.h"
#include "export.h"

#define crypto_auth_hmacsha256_BYTES 32U
#define crypto_auth_hmacsha256_KEYBYTES 32U

#ifdef __cplusplus
# if __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

typedef struct crypto_auth_hmacsha256_state {
    crypto_hash_sha256_state ictx;
    crypto_hash_sha256_state octx;
} crypto_auth_hmacsha256_state;

SODIUM_EXPORT
size_t crypto_auth_hmacsha256_bytes(void);

SODIUM_EXPORT
size_t crypto_auth_hmacsha256_keybytes(void);

SODIUM_EXPORT
const char * crypto_auth_hmacsha256_primitive(void);

SODIUM_EXPORT
int crypto_auth_hmacsha256(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);

SODIUM_EXPORT
int crypto_auth_hmacsha256_verify(const unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);

SODIUM_EXPORT
int crypto_auth_hmacsha256_init(crypto_auth_hmacsha256_state *state,
                                const unsigned char *key,
                                size_t keylen);

SODIUM_EXPORT
int crypto_auth_hmacsha256_update(crypto_auth_hmacsha256_state *state,
                                  const unsigned char *in,
                                  unsigned long long inlen);

SODIUM_EXPORT
int crypto_auth_hmacsha256_final(crypto_auth_hmacsha256_state *state,
                                 unsigned char *out);

#ifdef __cplusplus
}
#endif

#endif
