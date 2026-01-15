#include "crypto_kem.h"

size_t
crypto_kem_publickeybytes(void)
{
    return crypto_kem_PUBLICKEYBYTES;
}

size_t
crypto_kem_secretkeybytes(void)
{
    return crypto_kem_SECRETKEYBYTES;
}

size_t
crypto_kem_ciphertextbytes(void)
{
    return crypto_kem_CIPHERTEXTBYTES;
}

size_t
crypto_kem_sharedsecretbytes(void)
{
    return crypto_kem_SHAREDSECRETBYTES;
}

size_t
crypto_kem_seedbytes(void)
{
    return crypto_kem_SEEDBYTES;
}

const char *
crypto_kem_primitive(void)
{
    return crypto_kem_PRIMITIVE;
}

int
crypto_kem_seed_keypair(unsigned char *pk, unsigned char *sk,
                        const unsigned char *seed)
{
    return crypto_kem_xwing_seed_keypair(pk, sk, seed);
}

int
crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{
    return crypto_kem_xwing_keypair(pk, sk);
}

int
crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk)
{
    return crypto_kem_xwing_enc(ct, ss, pk);
}

int
crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk)
{
    return crypto_kem_xwing_dec(ss, ct, sk);
}
