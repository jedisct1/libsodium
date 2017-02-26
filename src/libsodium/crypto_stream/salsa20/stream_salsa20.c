#if defined(HAVE_EMMINTRIN_H) || \
    (defined(_MSC_VER) &&        \
     (defined(_M_X64) || defined(_M_AMD64) || defined(_M_IX86)))
# define INCLUDE_SSE2_IMPL
#endif

#include "crypto_stream_salsa20.h"
#include "stream_salsa20.h"
#include "randombytes.h"
#include "runtime.h"
#include "ref/stream_salsa20_ref.h"
#ifdef INCLUDE_SSE2_IMPL
# include "xmm6int/stream_salsa20_xmm6int.h"
#endif

#if defined(INCLUDE_SSE2_IMPL) && defined(__x86_64__)
static const crypto_stream_salsa20_implementation *implementation =
    &crypto_stream_salsa20_xmm6int_implementation;
#else
static const crypto_stream_salsa20_implementation *implementation =
    &crypto_stream_salsa20_ref_implementation;
#endif

size_t
crypto_stream_salsa20_keybytes(void)
{
    return crypto_stream_salsa20_KEYBYTES;
}

size_t
crypto_stream_salsa20_noncebytes(void)
{
    return crypto_stream_salsa20_NONCEBYTES;
}

int
crypto_stream_salsa20(unsigned char *c, unsigned long long clen,
                      const unsigned char *n, const unsigned char *k)
{
    return implementation->stream(c, clen, n, k);
}

int
crypto_stream_salsa20_xor_ic(unsigned char *c, const unsigned char *m,
                             unsigned long long mlen,
                             const unsigned char *n, uint64_t ic,
                             const unsigned char *k)
{
    return implementation->stream_xor_ic(c, m, mlen, n, ic, k);
}

int
crypto_stream_salsa20_xor(unsigned char *c, const unsigned char *m,
                          unsigned long long mlen, const unsigned char *n,
                          const unsigned char *k)
{
    return implementation->stream_xor_ic(c, m, mlen, n, 0U, k);
}

void
crypto_stream_salsa20_keygen(unsigned char k[crypto_stream_salsa20_KEYBYTES])
{
    randombytes_buf(k, crypto_stream_salsa20_KEYBYTES);
}

int
_crypto_stream_salsa20_pick_best_implementation(void)
{
#if defined(INCLUDE_SSE2_IMPL) && defined(__x86_64__)
    implementation = &crypto_stream_salsa20_xmm6int_implementation;
#else
    implementation = &crypto_stream_salsa20_ref_implementation;
#endif

#ifdef INCLUDE_SSE2_IMPL
    if (sodium_runtime_has_sse2()) {
        implementation = &crypto_stream_salsa20_xmm6int_implementation;
    }
#endif
    return 0;
}
