#ifndef crypto_vrf_H
#define crypto_vrf_H

/*
 * THREAD SAFETY: crypto_vrf_keypair() is thread-safe,
 * provided that sodium_init() was called before.
 *
 * Other functions, including crypto_vrf_keypair_from_seed(), are always thread-safe.
 */

#include <stddef.h>

#include "crypto_vrf_rfc9381.h"
#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_vrf_PROOFBYTES crypto_vrf_rfc9381_BYTES
SODIUM_EXPORT
size_t crypto_vrf_bytes(void);

#define crypto_vrf_OUTPUTBYTES crypto_vrf_rfc9381_OUTPUTBYTES
SODIUM_EXPORT
size_t crypto_vrf_outputbytes(void);

#define crypto_vrf_SEEDBYTES crypto_vrf_rfc9381_SEEDBYTES
SODIUM_EXPORT
size_t crypto_vrf_seedbytes(void);

#define crypto_vrf_PUBLICKEYBYTES crypto_vrf_rfc9381_PUBLICKEYBYTES
SODIUM_EXPORT
size_t crypto_vrf_publickeybytes(void);

#define crypto_vrf_SECRETKEYBYTES crypto_vrf_rfc9381_SECRETKEYBYTES
SODIUM_EXPORT
size_t crypto_vrf_secretkeybytes(void);

#define crypto_vrf_PRIMITIVE "rfc9381"
SODIUM_EXPORT
const char *crypto_vrf_primitive(void);

SODIUM_EXPORT
int crypto_vrf_keypair(unsigned char *pk, unsigned char *sk)
                       __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_vrf_seed_keypair(unsigned char *pk, unsigned char *sk,
				            const unsigned char *seed)
                            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_vrf_prove(unsigned char *proof, const unsigned char *m,
                     unsigned long long mlen, const unsigned char *sk)
                     __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_vrf_verify(unsigned char *output,
                      const unsigned char *pk,
                      const unsigned char *proof,
                      const unsigned char *m, unsigned long long mlen)
                      __attribute__ ((warn_unused_result))  __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_vrf_proof_to_hash(unsigned char *hash, const unsigned char *proof);

#ifdef __cplusplus
}
#endif

#endif
