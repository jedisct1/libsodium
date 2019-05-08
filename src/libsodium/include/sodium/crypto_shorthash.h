#ifndef crypto_shorthash_H
#define crypto_shorthash_H

#include <stddef.h>

#include "crypto_shorthash_siphash24.h"
#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_shorthash_BYTES crypto_shorthash_siphash24_BYTES
SODIUM_EXPORT
size_t  crypto_shorthash_bytes(void);

#define crypto_shorthash_KEYBYTES crypto_shorthash_siphash24_KEYBYTES
SODIUM_EXPORT
size_t  crypto_shorthash_keybytes(void);

#define crypto_shorthash_PRIMITIVE "siphash24"
SODIUM_EXPORT
const char *crypto_shorthash_primitive(void);

SODIUM_EXPORT
int crypto_shorthash(unsigned char *out, const unsigned char *in,
                     unsigned long long inlen, const unsigned char *k);

SODIUM_EXPORT
void crypto_shorthash_keygen(unsigned char k[crypto_shorthash_KEYBYTES]);

typedef crypto_shorthash_siphash24_state crypto_shorthash_state;

SODIUM_EXPORT
int crypto_shorthash_init(crypto_shorthash_state *state,
                          const unsigned char *k);

SODIUM_EXPORT
int crypto_shorthash_update(crypto_shorthash_state *state,
                            const unsigned char *in,
                            unsigned long long inlen);

SODIUM_EXPORT
int crypto_shorthash_final(crypto_shorthash_state *state,
                           unsigned char *out);

#ifdef __cplusplus
}
#endif

#endif
