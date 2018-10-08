
#include <string.h>

#include "crypto_hash_sha512.h"
#include "crypto_vrf_ietfdraft03.h"
#include "private/ed25519_ref10.h"
#include "randombytes.h"
#include "utils.h"

int
crypto_vrf_ietfdraft03_keypair(unsigned char pk[crypto_vrf_ietfdraft03_PUBLICKEYBYTES],
			       unsigned char sk[crypto_vrf_ietfdraft03_SECRETKEYBYTES])
{
    unsigned char seed[32];

    randombytes_buf(seed, sizeof seed);
    crypto_vrf_ietfdraft03_keypair_from_seed(pk, sk, seed);
    sodium_memzero(seed, sizeof seed);

    return 0;
}

int
crypto_vrf_ietfdraft03_keypair_from_seed(unsigned char pk[crypto_vrf_ietfdraft03_PUBLICKEYBYTES],
					 unsigned char sk[crypto_vrf_ietfdraft03_SECRETKEYBYTES],
					 const unsigned char seed[crypto_vrf_ietfdraft03_SEEDBYTES])
{
    ge25519_p3 A;

    crypto_hash_sha512(sk, seed, 32);
    sk[0] &= 248;
    sk[31] &= 127;
    sk[31] |= 64;
    ge25519_scalarmult_base(&A, sk);
    ge25519_p3_tobytes(pk, &A);
    memmove(sk, seed, 32);
    memmove(sk + 32, pk, 32);

    return 0;
}

void
crypto_vrf_ietfdraft03_sk_to_pk(unsigned char pk[crypto_vrf_ietfdraft03_PUBLICKEYBYTES],
				const unsigned char skpk[crypto_vrf_ietfdraft03_SECRETKEYBYTES])
{
    memmove(pk, skpk+32, 32);
}

void
crypto_vrf_ietfdraft03_sk_to_seed(unsigned char seed[crypto_vrf_ietfdraft03_SEEDBYTES],
				  const unsigned char skpk[crypto_vrf_ietfdraft03_SECRETKEYBYTES])
{
    memmove(seed, skpk, 32);
}
