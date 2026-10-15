#include "crypto_sign_mldsa44.h"
#include "ref/sign_mldsa44_ref.h"

size_t
crypto_sign_mldsa44_bytes(void)
{
    return crypto_sign_mldsa44_BYTES;
}

size_t
crypto_sign_mldsa44_publickeybytes(void)
{
    return crypto_sign_mldsa44_PUBLICKEYBYTES;
}

size_t
crypto_sign_mldsa44_secretkeybytes(void)
{
    return crypto_sign_mldsa44_SECRETKEYBYTES;
}

size_t
crypto_sign_mldsa44_seedbytes(void)
{
    return crypto_sign_mldsa44_SEEDBYTES;
}

size_t
crypto_sign_mldsa44_contextbytes_max(void)
{
    return crypto_sign_mldsa44_CONTEXTBYTES_MAX;
}

size_t
crypto_sign_mldsa44_messagebytes_max(void)
{
    return crypto_sign_mldsa44_MESSAGEBYTES_MAX;
}

int
crypto_sign_mldsa44_seed_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *seed)
{
    return mldsa44_ref_seed_keypair(pk, sk, seed);
}

int
crypto_sign_mldsa44_keypair(unsigned char *pk, unsigned char *sk)
{
    return mldsa44_ref_keypair(pk, sk);
}

int
crypto_sign_mldsa44_detached(unsigned char *sig, unsigned long long *siglen_p,
                             const unsigned char *m, unsigned long long mlen,
                             const unsigned char *sk)
{
    return mldsa44_ref_sign_detached(sig, siglen_p, m, mlen, NULL, 0, sk);
}

int
crypto_sign_mldsa44_detached_ctx(unsigned char *sig, unsigned long long *siglen_p,
                                 const unsigned char *m, unsigned long long mlen,
                                 const unsigned char *ctx, size_t ctxlen, const unsigned char *sk)
{
    return mldsa44_ref_sign_detached(sig, siglen_p, m, mlen, ctx, ctxlen, sk);
}

int
crypto_sign_mldsa44_verify_detached(const unsigned char *sig, const unsigned char *m,
                                    unsigned long long mlen, const unsigned char *pk)
{
    return mldsa44_ref_verify_detached(sig, m, mlen, NULL, 0, pk);
}

int
crypto_sign_mldsa44_verify_detached_ctx(const unsigned char *sig, const unsigned char *m,
                                        unsigned long long mlen, const unsigned char *ctx,
                                        size_t ctxlen, const unsigned char *pk)
{
    return mldsa44_ref_verify_detached(sig, m, mlen, ctx, ctxlen, pk);
}
