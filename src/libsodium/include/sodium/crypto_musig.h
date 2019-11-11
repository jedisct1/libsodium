#ifndef crypto_musig_H
#define crypto_musig_H

/*
 * This is an implementation of the MuSig algorithm described in
 * "Simple Schnorr Multi-Signatures with Applications to Bitcoin" by
 * Gregory Maxwell, Andrew Poelstra, Yannick Seurin, and Pieter Wuille
 * https://eprint.iacr.org/2018/068.pdf
 *
 * blake2b is used for hashing and ristretto255 as the underlying group.
 *
 * See tests/default/musig.c for example usage
 */


/*
 * THREAD SAFETY: crypto_musig_keypair() and crypto_musig_begin() are
 * thread-safe provided that sodium_init() was called before.
 *
 * Other functions, including crypto_musig_seed_keypair() are always thread-safe.
 */

#include <stddef.h>

#include "crypto_core_ristretto255.h"
#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

SODIUM_EXPORT
size_t  crypto_musig_statebytes(void);

#define crypto_musig_BYTES (crypto_musig_RBYTES + \
        crypto_musig_KSTBYTES)
SODIUM_EXPORT
size_t  crypto_musig_bytes(void);

#define crypto_musig_SEEDBYTES crypto_core_ristretto255_SCALARBYTES
SODIUM_EXPORT
size_t  crypto_musig_seedbytes(void);

#define crypto_musig_PUBLICKEYBYTES crypto_core_ristretto255_BYTES
SODIUM_EXPORT
size_t  crypto_musig_publickeybytes(void);

#define crypto_musig_SECRETKEYBYTES crypto_core_ristretto255_SCALARBYTES
SODIUM_EXPORT
size_t  crypto_musig_secretkeybytes(void);

#define crypto_musig_RBYTES crypto_core_ristretto255_BYTES
SODIUM_EXPORT
size_t  crypto_musig_rbytes(void);

#define crypto_musig_KSTBYTES crypto_core_ristretto255_SCALARBYTES
SODIUM_EXPORT
size_t  crypto_musig_kstbytes(void);

#define crypto_musig_MESSAGEBYTES_MAX (SODIUM_SIZE_MAX - \
        crypto_musig_BYTES)
SODIUM_EXPORT
size_t  crypto_musig_messagebytes_max(void);

#define crypto_musig_PRIMITIVE "musig_ristretto255"
SODIUM_EXPORT
const char *crypto_musig_primitive(void);

typedef struct crypto_musig_peer {
    unsigned char pk[crypto_musig_PUBLICKEYBYTES];
    unsigned char r[crypto_musig_RBYTES];
    unsigned char s[crypto_musig_KSTBYTES];
    unsigned char t[crypto_musig_KSTBYTES];
    struct crypto_musig_peer *next;
    unsigned char is_self;
} crypto_musig_peer;

typedef struct crypto_musig_state {
    unsigned char k[crypto_musig_KSTBYTES];
    crypto_musig_peer *peers;
} crypto_musig_state;

SODIUM_EXPORT
int crypto_musig_seed_keypair(unsigned char *pk, unsigned char *sk,
                              const unsigned char *seed)
    __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_musig_keypair(unsigned char *pk, unsigned char *sk)
    __attribute__ ((nonnull));

SODIUM_EXPORT
void crypto_musig_init(crypto_musig_state *st)
    __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_musig_add(crypto_musig_state *st, const unsigned char *pk)
    __attribute__ ((nonnull));

SODIUM_EXPORT
void crypto_musig_begin(crypto_musig_state *st, unsigned char *r,
                        unsigned char *t) __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_musig_commit(crypto_musig_state *st, const unsigned char *pk,
                        const unsigned char *t) __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_musig_check(crypto_musig_state *st, const unsigned char *pk,
                       const unsigned char *r) __attribute__ ((nonnull))
    __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_musig_update(crypto_musig_state *st, const unsigned char *pk,
                        const unsigned char *s) __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_musig_sign(crypto_musig_state *st, unsigned char *s,
                      const unsigned char *sk, const unsigned char *m,
                      unsigned long long mlen)
    __attribute__ ((nonnull(1, 2, 3))) __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_musig_key(crypto_musig_state *st, unsigned char *pk)
    __attribute__ ((nonnull)) __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_musig_final(crypto_musig_state *st, unsigned char *sm,
                       unsigned long long *smlen_p, const unsigned char *m,
                       unsigned long long mlen)
    __attribute__ ((nonnull(1, 2))) __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_musig_final_detached(crypto_musig_state *st, unsigned char *sig,
                                const unsigned char *m, unsigned long long mlen)
    __attribute__ ((nonnull(1, 2))) __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_musig_verify(unsigned char *m, unsigned long long *mlen_p,
                        const unsigned char *sm, unsigned long long smlen,
                        const unsigned char *pk)
    __attribute__ ((nonnull(3, 5))) __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_musig_verify_detached(const unsigned char *sig,
                                 const unsigned char *m,
                                 unsigned long long mlen,
                                 const unsigned char *pk)
    __attribute__ ((nonnull(1, 4))) __attribute__ ((warn_unused_result)) ;

SODIUM_EXPORT
void crypto_musig_cleanup(crypto_musig_state *state)
    __attribute__ ((nonnull));

#ifdef __cplusplus
}
#endif

#endif
