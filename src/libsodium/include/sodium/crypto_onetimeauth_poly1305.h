#ifndef crypto_onetimeauth_poly1305_H
#define crypto_onetimeauth_poly1305_H

#include <stddef.h>
#include "export.h"

#define crypto_onetimeauth_poly1305_BYTES 16U
#define crypto_onetimeauth_poly1305_KEYBYTES 32U
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>

typedef struct crypto_onetimeauth_poly1305_implementation {
    const char *(*implementation_name)(void);
    int         (*onetimeauth)(unsigned char *out,
                               const unsigned char *in,
                               unsigned long long inlen,
                               const unsigned char *k);
    int         (*onetimeauth_verify)(const unsigned char *h,
                                      const unsigned char *in,
                                      unsigned long long inlen,
                                      const unsigned char *k);
} crypto_onetimeauth_poly1305_implementation;

SODIUM_EXPORT
size_t crypto_onetimeauth_poly1305_bytes(void);

SODIUM_EXPORT
size_t crypto_onetimeauth_poly1305_keybytes(void);

SODIUM_EXPORT
const char * crypto_onetimeauth_poly1305_primitive(void);

SODIUM_EXPORT
const char *crypto_onetimeauth_poly1305_implementation_name(void);

SODIUM_EXPORT
int crypto_onetimeauth_poly1305_set_implementation(crypto_onetimeauth_poly1305_implementation *impl);

SODIUM_EXPORT
crypto_onetimeauth_poly1305_implementation *
        crypto_onetimeauth_pick_best_implementation(void);

SODIUM_EXPORT
int crypto_onetimeauth_poly1305(unsigned char *out,
                                const unsigned char *in,
                                unsigned long long inlen,
                                const unsigned char *k);

SODIUM_EXPORT
int crypto_onetimeauth_poly1305_verify(const unsigned char *h,
                                       const unsigned char *in,
                                       unsigned long long inlen,
                                       const unsigned char *k);

#ifdef __cplusplus
}
#endif

#endif
