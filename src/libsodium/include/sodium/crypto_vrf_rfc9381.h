
#ifndef crypto_vrf_rfc9381_H
#define crypto_vrf_rfc9381_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_vrf_rfc9381_BYTES 80U
SODIUM_EXPORT
size_t crypto_vrf_rfc9381_bytes(void);

#define crypto_vrf_rfc9381_OUTPUTBYTES 64U
SODIUM_EXPORT
size_t crypto_vrf_rfc9381_outputbytes(void);

#define crypto_vrf_rfc9381_SEEDBYTES 32U
SODIUM_EXPORT
size_t crypto_vrf_rfc9381_seedbytes(void);

#define crypto_vrf_rfc9381_PUBLICKEYBYTES 32U
SODIUM_EXPORT
size_t crypto_vrf_rfc9381_publickeybytes(void);

#define crypto_vrf_rfc9381_SECRETKEYBYTES 64U
SODIUM_EXPORT
size_t crypto_vrf_rfc9381_secretkeybytes(void);

SODIUM_EXPORT
int crypto_vrf_rfc9381_prove(unsigned char *proof,
				 const unsigned char *m,
				 unsigned long long mlen, const unsigned char *sk);

SODIUM_EXPORT
int crypto_vrf_rfc9381_verify(unsigned char *output,
				  const unsigned char *pk,
				  const unsigned char *proof,
				  const unsigned char *m,
				  unsigned long long mlen)
            __attribute__ ((warn_unused_result)) __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_vrf_rfc9381_proof_to_hash(unsigned char *hash,
				         const unsigned char *proof)
                         __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_vrf_rfc9381_keypair(unsigned char *pk, unsigned char *sk)
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_vrf_rfc9381_seed_keypair(unsigned char *pk,
                                             unsigned char *sk,
                                             const unsigned char *seed)
                                             __attribute__ ((nonnull));

SODIUM_EXPORT
void crypto_vrf_rfc9381_sk_to_seed(unsigned char *seed,
                                       const unsigned char *sk)
                                       __attribute__ ((nonnull));

SODIUM_EXPORT
void crypto_vrf_rfc9381_sk_to_pk(unsigned char *pk,
				     const unsigned char *sk)
                     __attribute__ ((nonnull));

#ifdef __cplusplus
}
#endif

#endif
