
#include "crypto_vrf.h"

size_t
crypto_vrf_publickeybytes(void)
{
    return crypto_vrf_PUBLICKEYBYTES;
}

size_t
crypto_vrf_secretkeybytes(void)
{
    return crypto_vrf_SECRETKEYBYTES;
}

size_t
crypto_vrf_seedbytes(void)
{
    return crypto_vrf_SEEDBYTES;
}

size_t
crypto_vrf_proofbytes(void)
{
    return crypto_vrf_PROOFBYTES;
}

size_t
crypto_vrf_outputbytes(void)
{
    return crypto_vrf_OUTPUTBYTES;
}

const char *
crypto_vrf_primitive(void)
{
    return crypto_vrf_PRIMITIVE;
}

int
crypto_vrf_keypair(unsigned char *pk, unsigned char *sk)
{
    return crypto_vrf_ietfdraft03_keypair(pk, sk);
}

int
crypto_vrf_keypair_from_seed(unsigned char *pk, unsigned char *sk,
			     const unsigned char *seed)
{
    return crypto_vrf_ietfdraft03_keypair_from_seed(pk, sk, seed);
}

int
crypto_vrf_is_valid_key(const unsigned char *pk)
{
    return crypto_vrf_ietfdraft03_is_valid_key(pk);
}

int
crypto_vrf_prove(unsigned char *proof, const unsigned char *skpk,
		 const unsigned char *m, const unsigned long long mlen)
{
    return crypto_vrf_ietfdraft03_prove(proof, skpk, m, mlen);
}

int
crypto_vrf_verify(unsigned char *output, const unsigned char *pk,
		  const unsigned char *proof, const unsigned char *m,
		  const unsigned long long mlen)
{
    return crypto_vrf_ietfdraft03_verify(output, pk, proof, m, mlen);
}

int
crypto_vrf_proof_to_hash(unsigned char *hash, const unsigned char *proof)
{
    return crypto_vrf_ietfdraft03_proof_to_hash(hash, proof);
}

void
crypto_vrf_sk_to_pk(unsigned char *pk, const unsigned char *skpk)
{
    crypto_vrf_ietfdraft03_sk_to_pk(pk, skpk);
}

void
crypto_vrf_sk_to_seed(unsigned char *seed, const unsigned char *skpk)
{
    crypto_vrf_ietfdraft03_sk_to_seed(seed, skpk);
}
