#ifndef crypto_box_H
#define crypto_box_H

/*
 * THREAD SAFETY: crypto_box_keypair() is thread-safe,
 * provided that you called sodium_init() once before using any
 * other libsodium function.
 * Other functions are always thread-safe.
 */

#include <stddef.h>
#include <stdint.h>

#include "crypto_box_curve25519xsalsa20poly1305.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_box_PUBLICKEYBYTES crypto_box_curve25519xsalsa20poly1305_PUBLICKEYBYTES
SODIUM_EXPORT
size_t  crypto_box_publickeybytes(void);

#define crypto_box_SECRETKEYBYTES crypto_box_curve25519xsalsa20poly1305_SECRETKEYBYTES
SODIUM_EXPORT
size_t  crypto_box_secretkeybytes(void);

#define crypto_box_BEFORENMBYTES crypto_box_curve25519xsalsa20poly1305_BEFORENMBYTES
SODIUM_EXPORT
size_t  crypto_box_beforenmbytes(void);

#define crypto_box_NONCEBYTES crypto_box_curve25519xsalsa20poly1305_NONCEBYTES
SODIUM_EXPORT
size_t  crypto_box_noncebytes(void);

#define crypto_box_ZEROBYTES crypto_box_curve25519xsalsa20poly1305_ZEROBYTES
SODIUM_EXPORT
size_t  crypto_box_zerobytes(void);

#define crypto_box_BOXZEROBYTES crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES
SODIUM_EXPORT
size_t  crypto_box_boxzerobytes(void);

#define crypto_box_MACBYTES crypto_box_curve25519xsalsa20poly1305_MACBYTES
SODIUM_EXPORT
size_t  crypto_box_macbytes(void);

#define crypto_box_PRIMITIVE "curve25519xsalsa20poly1305"
SODIUM_EXPORT
const char *crypto_box_primitive(void);

SODIUM_EXPORT
int crypto_box_keypair(unsigned char *pk, unsigned char *sk);

SODIUM_EXPORT
int crypto_box_beforenm(unsigned char *k, const unsigned char *pk,
                        const unsigned char *sk);

SODIUM_EXPORT
int crypto_box_afternm(unsigned char *c, const unsigned char *m,
                       uint64_t mlen, const unsigned char *n,
                       const unsigned char *k);

SODIUM_EXPORT
int crypto_box_open_afternm(unsigned char *m, const unsigned char *c,
                            uint64_t clen, const unsigned char *n,
                            const unsigned char *k);

SODIUM_EXPORT
int crypto_box(unsigned char *c, const unsigned char *m,
               uint64_t mlen, const unsigned char *n,
               const unsigned char *pk, const unsigned char *sk);

SODIUM_EXPORT
int crypto_box_open(unsigned char *m, const unsigned char *c,
                    uint64_t clen, const unsigned char *n,
                    const unsigned char *pk, const unsigned char *sk);

#ifdef __cplusplus
}
#endif

#endif
