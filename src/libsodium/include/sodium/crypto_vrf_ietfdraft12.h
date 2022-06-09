
#ifndef crypto_vrf_ietfdraft12_H
#define crypto_vrf_ietfdraft12_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_vrf_ietfdraft12_BYTES 80U
SODIUM_EXPORT
size_t crypto_vrf_ietfdraft12_bytes(void);

#define crypto_vrf_ietfdraft12_OUTPUTBYTES 64U
SODIUM_EXPORT
size_t crypto_vrf_ietfdraft12_outputbytes(void);

#define crypto_vrf_ietfdraft12_SEEDBYTES 32U
SODIUM_EXPORT
size_t crypto_vrf_ietfdraft12_seedbytes(void);

#define crypto_vrf_ietfdraft12_PUBLICKEYBYTES 32U
SODIUM_EXPORT
size_t crypto_vrf_ietfdraft12_publickeybytes(void);

#define crypto_vrf_ietfdraft12_SECRETKEYBYTES 64U
SODIUM_EXPORT
size_t crypto_vrf_ietfdraft12_secretkeybytes(void);

SODIUM_EXPORT
int crypto_vrf_ietfdraft12_prove(unsigned char *proof,
				 const unsigned char *m,
				 unsigned long long mlen, const unsigned char *sk);

SODIUM_EXPORT
int crypto_vrf_ietfdraft12_verify(unsigned char *output,
				  const unsigned char *pk,
				  const unsigned char *proof,
				  const unsigned char *m,
				  unsigned long long mlen)
            __attribute__ ((warn_unused_result)) __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_vrf_ietfdraft12_proof_to_hash(unsigned char *hash,
				         const unsigned char *proof)
                         __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_vrf_ietfdraft12_keypair(unsigned char *pk, unsigned char *sk)
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_vrf_ietfdraft12_seed_keypair(unsigned char *pk,
                                             unsigned char *sk,
                                             const unsigned char *seed)
                                             __attribute__ ((nonnull));

SODIUM_EXPORT
void crypto_vrf_ietfdraft12_sk_to_seed(unsigned char *seed,
                                       const unsigned char *sk)
                                       __attribute__ ((nonnull));

SODIUM_EXPORT
void crypto_vrf_ietfdraft12_sk_to_pk(unsigned char *pk,
				     const unsigned char *sk)
                     __attribute__ ((nonnull));

#ifdef __cplusplus
}
#endif

#endif
