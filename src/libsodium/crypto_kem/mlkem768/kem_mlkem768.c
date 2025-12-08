#include "crypto_kem_mlkem768.h"
#include "ref/kem_mlkem768_ref.h"

size_t
crypto_kem_mlkem768_publickeybytes(void)
{
    return crypto_kem_mlkem768_PUBLICKEYBYTES;
}

size_t
crypto_kem_mlkem768_secretkeybytes(void)
{
    return crypto_kem_mlkem768_SECRETKEYBYTES;
}

size_t
crypto_kem_mlkem768_ciphertextbytes(void)
{
    return crypto_kem_mlkem768_CIPHERTEXTBYTES;
}

size_t
crypto_kem_mlkem768_sharedsecretbytes(void)
{
    return crypto_kem_mlkem768_SHAREDSECRETBYTES;
}

size_t
crypto_kem_mlkem768_seedbytes(void)
{
    return crypto_kem_mlkem768_SEEDBYTES;
}

int
crypto_kem_mlkem768_seed_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *seed)
{
    return mlkem768_ref_seed_keypair(pk, sk, seed);
}

int
crypto_kem_mlkem768_keypair(unsigned char *pk, unsigned char *sk)
{
    return mlkem768_ref_keypair(pk, sk);
}

int
crypto_kem_mlkem768_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk)
{
    return mlkem768_ref_enc(ct, ss, pk);
}

int
crypto_kem_mlkem768_enc_deterministic(unsigned char *ct, unsigned char *ss, const unsigned char *pk,
                                      const unsigned char *seed)
{
    return mlkem768_ref_enc_deterministic(ct, ss, pk, seed);
}

int
crypto_kem_mlkem768_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk)
{
    return mlkem768_ref_dec(ss, ct, sk);
}
