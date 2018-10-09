#ifndef crypto_vrf_H
#define crypto_vrf_H

/*
 * THREAD SAFETY: crypto_vrf_keypair() is thread-safe provided that
 * sodium_init() was called before.
 *
 * Other functions, including crypto_vrf_keypair_from_seed(), are always
 * thread-safe.
 */

#include <stddef.h>

#include "crypto_vrf_ietfdraft03.h"
#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_vrf_PUBLICKEYBYTES crypto_vrf_ietfdraft03_PUBLICKEYBYTES
SODIUM_EXPORT
size_t crypto_vrf_publickeybytes(void);

#define crypto_vrf_SECRETKEYBYTES crypto_vrf_ietfdraft03_SECRETKEYBYTES
SODIUM_EXPORT
size_t crypto_vrf_secretkeybytes(void);

#define crypto_vrf_SEEDBYTES crypto_vrf_ietfdraft03_SEEDBYTES
SODIUM_EXPORT
size_t crypto_vrf_seedbytes(void);

#define crypto_vrf_PROOFBYTES crypto_vrf_ietfdraft03_PROOFBYTES
SODIUM_EXPORT
size_t crypto_vrf_proofbytes(void);

#define crypto_vrf_OUTPUTBYTES crypto_vrf_ietfdraft03_OUTPUTBYTES
SODIUM_EXPORT
size_t crypto_vrf_outputbytes(void);

#define crypto_vrf_PRIMITIVE "ietfdraft03"
SODIUM_EXPORT
const char *crypto_vrf_primitive(void);

SODIUM_EXPORT
int crypto_vrf_keypair(unsigned char *pk, unsigned char *sk);

SODIUM_EXPORT
int crypto_vrf_keypair_from_seed(unsigned char *pk, unsigned char *sk,
				 const unsigned char *seed);

SODIUM_EXPORT
int crypto_vrf_is_valid_key(const unsigned char *pk)
            __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_vrf_prove(unsigned char *proof, const unsigned char *sk,
		     const unsigned char *m, unsigned long long mlen);

SODIUM_EXPORT
int crypto_vrf_verify(unsigned char *output,
		      const unsigned char *pk,
		      const unsigned char *proof,
		      const unsigned char *m, unsigned long long mlen)
            __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_vrf_proof_to_hash(unsigned char *hash, const unsigned char *proof);

SODIUM_EXPORT
void crypto_vrf_sk_to_pk(unsigned char *pk, const unsigned char *skpk);

SODIUM_EXPORT
void crypto_vrf_sk_to_seed(unsigned char *seed, const unsigned char *skpk);

#ifdef __cplusplus
}
#endif

#endif
