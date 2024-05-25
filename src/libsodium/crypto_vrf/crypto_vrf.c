
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
    return crypto_vrf_rfc9381_keypair(pk, sk);
}

int
crypto_vrf_seed_keypair(unsigned char *pk, unsigned char *sk,
			     const unsigned char *seed)
{
    return crypto_vrf_rfc9381_seed_keypair(pk, sk, seed);
}

int
crypto_vrf_prove(unsigned char *proof, const unsigned char *m, const unsigned long long mlen,
                 const unsigned char *skpk)
{
    return crypto_vrf_rfc9381_prove(proof, m, mlen, skpk);
}

int
crypto_vrf_verify(unsigned char *output, const unsigned char *pk,
		  const unsigned char *proof, const unsigned char *m,
		  const unsigned long long mlen)
{
    return crypto_vrf_rfc9381_verify(output, pk, proof, m, mlen);
}

int
crypto_vrf_proof_to_hash(unsigned char *hash, const unsigned char *proof)
{
    return crypto_vrf_rfc9381_proof_to_hash(hash, proof);
}
