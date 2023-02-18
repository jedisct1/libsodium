#include "crypto_stream_forro14.h"
#include "core.h"
#include "private/forro14_ietf_ext.h"
#include "private/common.h"
#include "private/implementations.h"
#include "randombytes.h"
#include "runtime.h"
#include "stream_forro14.h"

#include "ref/forro14_ref.h"
#if defined(HAVE_AVX2INTRIN_H) && defined(HAVE_EMMINTRIN_H) && \
    defined(HAVE_TMMINTRIN_H) && defined(HAVE_SMMINTRIN_H)
#include "dolbeau/forro14_dolbeau-avx2.h"
#endif
#if defined(HAVE_EMMINTRIN_H) && defined(HAVE_TMMINTRIN_H)
#include "dolbeau/forro14_dolbeau-ssse3.h"
#endif

static const crypto_stream_forro14_implementation *implementation =
    &crypto_stream_forro14_ref_implementation;

size_t
crypto_stream_forro14_keybytes(void)
{
    return crypto_stream_forro14_KEYBYTES;
}

size_t
crypto_stream_forro14_noncebytes(void)
{
    return crypto_stream_forro14_NONCEBYTES;
}

size_t
crypto_stream_forro14_messagebytes_max(void)
{
    return crypto_stream_forro14_MESSAGEBYTES_MAX;
}

size_t
crypto_stream_forro14_ietf_keybytes(void)
{
    return crypto_stream_forro14_ietf_KEYBYTES;
}

size_t
crypto_stream_forro14_ietf_noncebytes(void)
{
    return crypto_stream_forro14_ietf_NONCEBYTES;
}

size_t
crypto_stream_forro14_ietf_messagebytes_max(void)
{
    return crypto_stream_forro14_ietf_MESSAGEBYTES_MAX;
}

int crypto_stream_forro14(unsigned char *c, unsigned long long clen,
                          const unsigned char *n, const unsigned char *k)
{
    if (clen > crypto_stream_forro14_MESSAGEBYTES_MAX)
    {
        sodium_misuse();
    }
    return implementation->stream(c, clen, n, k);
}

int crypto_stream_forro14_xor_ic(unsigned char *c, const unsigned char *m,
                                 unsigned long long mlen,
                                 const unsigned char *n, uint64_t ic,
                                 const unsigned char *k)
{
    if (mlen > crypto_stream_forro14_MESSAGEBYTES_MAX)
    {
        sodium_misuse();
    }
    return implementation->stream_xor_ic(c, m, mlen, n, ic, k);
}

int crypto_stream_forro14_xor(unsigned char *c, const unsigned char *m,
                              unsigned long long mlen, const unsigned char *n,
                              const unsigned char *k)
{
    if (mlen > crypto_stream_forro14_MESSAGEBYTES_MAX)
    {
        sodium_misuse();
    }
    return implementation->stream_xor_ic(c, m, mlen, n, 0U, k);
}

int crypto_stream_forro14_ietf_ext(unsigned char *c, unsigned long long clen,
                                   const unsigned char *n, const unsigned char *k)
{
    if (clen > crypto_stream_forro14_MESSAGEBYTES_MAX)
    {
        sodium_misuse();
    }
    return implementation->stream_ietf_ext(c, clen, n, k);
}

int crypto_stream_forro14_ietf_ext_xor_ic(unsigned char *c, const unsigned char *m,
                                          unsigned long long mlen,
                                          const unsigned char *n, uint32_t ic,
                                          const unsigned char *k)
{
    if (mlen > crypto_stream_forro14_MESSAGEBYTES_MAX)
    {
        sodium_misuse();
    }
    return implementation->stream_ietf_ext_xor_ic(c, m, mlen, n, ic, k);
}

static int
crypto_stream_forro14_ietf_ext_xor(unsigned char *c, const unsigned char *m,
                                   unsigned long long mlen, const unsigned char *n,
                                   const unsigned char *k)
{
    if (mlen > crypto_stream_forro14_MESSAGEBYTES_MAX)
    {
        sodium_misuse();
    }
    return implementation->stream_ietf_ext_xor_ic(c, m, mlen, n, 0U, k);
}

int crypto_stream_forro14_ietf(unsigned char *c, unsigned long long clen,
                               const unsigned char *n, const unsigned char *k)
{
    if (clen > crypto_stream_forro14_ietf_MESSAGEBYTES_MAX)
    {
        sodium_misuse();
    }
    return crypto_stream_forro14_ietf_ext(c, clen, n, k);
}

int crypto_stream_forro14_ietf_xor_ic(unsigned char *c, const unsigned char *m,
                                      unsigned long long mlen,
                                      const unsigned char *n, uint32_t ic,
                                      const unsigned char *k)
{
    if ((unsigned long long)ic >
        (64ULL * (1ULL << 32)) / 64ULL - (mlen + 63ULL) / 64ULL)
    {
        sodium_misuse();
    }
    return crypto_stream_forro14_ietf_ext_xor_ic(c, m, mlen, n, ic, k);
}

int crypto_stream_forro14_ietf_xor(unsigned char *c, const unsigned char *m,
                                   unsigned long long mlen, const unsigned char *n,
                                   const unsigned char *k)
{
    if (mlen > crypto_stream_forro14_ietf_MESSAGEBYTES_MAX)
    {
        sodium_misuse();
    }
    return crypto_stream_forro14_ietf_ext_xor(c, m, mlen, n, k);
}

void crypto_stream_forro14_ietf_keygen(unsigned char k[crypto_stream_forro14_ietf_KEYBYTES])
{
    randombytes_buf(k, crypto_stream_forro14_ietf_KEYBYTES);
}

void crypto_stream_forro14_keygen(unsigned char k[crypto_stream_forro14_KEYBYTES])
{
    randombytes_buf(k, crypto_stream_forro14_KEYBYTES);
}

int _crypto_stream_forro14_pick_best_implementation(void)
{
    implementation = &crypto_stream_forro14_ref_implementation;
#if defined(HAVE_AVX2INTRIN_H) && defined(HAVE_EMMINTRIN_H) && \
    defined(HAVE_TMMINTRIN_H) && defined(HAVE_SMMINTRIN_H)
    if (sodium_runtime_has_avx2())
    {
        implementation = &crypto_stream_forro14_dolbeau_avx2_implementation;
        return 0;
    }
#endif
#if defined(HAVE_EMMINTRIN_H) && defined(HAVE_TMMINTRIN_H)
    if (sodium_runtime_has_ssse3())
    {
        implementation = &crypto_stream_forro14_dolbeau_ssse3_implementation;
        return 0;
    }
#endif
    return 0;
}
