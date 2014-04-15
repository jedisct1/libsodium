#ifndef crypto_auth_hmacsha512_H
#define crypto_auth_hmacsha512_H

#include <stddef.h>
#include "crypto_hash_sha512.h"
#include "export.h"

#define crypto_auth_hmacsha512_BYTES 64U
#define crypto_auth_hmacsha512_KEYBYTES 32U

#ifdef __cplusplus
# if __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

typedef struct crypto_auth_hmacsha512_state {
    crypto_hash_sha512_state ictx;
    crypto_hash_sha512_state octx;
} crypto_auth_hmacsha512_state;

SODIUM_EXPORT
size_t crypto_auth_hmacsha512_bytes(void);

SODIUM_EXPORT
size_t crypto_auth_hmacsha512_keybytes(void);

SODIUM_EXPORT
const char * crypto_auth_hmacsha512_primitive(void);

SODIUM_EXPORT
int crypto_auth_hmacsha512(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);

SODIUM_EXPORT
int crypto_auth_hmacsha512_verify(const unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);

SODIUM_EXPORT
int crypto_auth_hmacsha512_init(crypto_auth_hmacsha512_state *state,
                                const unsigned char *key,
                                size_t keylen);

SODIUM_EXPORT
int crypto_auth_hmacsha512_update(crypto_auth_hmacsha512_state *state,
                                  const unsigned char *in,
                                  unsigned long long inlen);

SODIUM_EXPORT
int crypto_auth_hmacsha512_final(crypto_auth_hmacsha512_state *state,
                                 unsigned char *out);
    
#ifdef __cplusplus
}
#endif

#endif
