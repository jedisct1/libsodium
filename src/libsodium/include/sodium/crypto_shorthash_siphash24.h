#ifndef crypto_shorthash_siphash24_H
#define crypto_shorthash_siphash24_H

#include <stddef.h>
#include <stdint.h>
#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

typedef struct CRYPTO_ALIGN(16) crypto_shorthash_siphash24_state {
    uint64_t       v0;
    uint64_t       v1;
    uint64_t       v2;
    uint64_t       v3;
    uint64_t       m;
    uint8_t        m_shift;
    uint8_t        len;
} crypto_shorthash_siphash24_state;

/* -- 64-bit output -- */

#define crypto_shorthash_siphash24_BYTES 8U
SODIUM_EXPORT
size_t crypto_shorthash_siphash24_bytes(void);

#define crypto_shorthash_siphash24_KEYBYTES 16U
SODIUM_EXPORT
size_t crypto_shorthash_siphash24_keybytes(void);

SODIUM_EXPORT
int crypto_shorthash_siphash24(unsigned char *out, const unsigned char *in,
                               unsigned long long inlen, const unsigned char *k);

SODIUM_EXPORT
int crypto_shorthash_siphash24_init(crypto_shorthash_siphash24_state *state,
                                    const unsigned char *k);

SODIUM_EXPORT
int crypto_shorthash_siphash24_update(crypto_shorthash_siphash24_state *state,
                                      const unsigned char *in,
                                      unsigned long long inlen);

SODIUM_EXPORT
int crypto_shorthash_siphash24_final(crypto_shorthash_siphash24_state *state,
                                     unsigned char *out);

#ifndef SODIUM_LIBRARY_MINIMAL
/* -- 128-bit output -- */

#define crypto_shorthash_siphashx24_BYTES 16U
SODIUM_EXPORT
size_t crypto_shorthash_siphashx24_bytes(void);

#define crypto_shorthash_siphashx24_KEYBYTES 16U
SODIUM_EXPORT
size_t crypto_shorthash_siphashx24_keybytes(void);

SODIUM_EXPORT
int crypto_shorthash_siphashx24(unsigned char *out, const unsigned char *in,
                                unsigned long long inlen, const unsigned char *k);
#endif

#ifdef __cplusplus
}
#endif

#endif
