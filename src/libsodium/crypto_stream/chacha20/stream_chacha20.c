#include "crypto_stream_chacha20.h"
#include "stream_chacha20.h"
#include "runtime.h"
#include "ref/stream_chacha20_ref.h"

static const crypto_stream_chacha20_implementation *implementation =
    &crypto_stream_chacha20_ref_implementation;

size_t
crypto_stream_chacha20_keybytes(void) {
    return crypto_stream_chacha20_KEYBYTES;
}

size_t
crypto_stream_chacha20_noncebytes(void) {
    return crypto_stream_chacha20_NONCEBYTES;
}

size_t
crypto_stream_chacha20_ietf_noncebytes(void) {
    return crypto_stream_chacha20_IETF_NONCEBYTES;
}

int
crypto_stream_chacha20(unsigned char *c, unsigned long long clen,
                       const unsigned char *n, const unsigned char *k)
{
    return implementation->stream(c, clen, n, k);
}

int
crypto_stream_chacha20_ietf(unsigned char *c, unsigned long long clen,
                            const unsigned char *n, const unsigned char *k)
{
    return implementation->stream_ietf(c, clen, n, k);
}

int
crypto_stream_chacha20_xor_ic(unsigned char *c, const unsigned char *m,
                              unsigned long long mlen,
                              const unsigned char *n, uint64_t ic,
                              const unsigned char *k)
{
    return implementation->stream_xor_ic(c, m, mlen, n, ic, k);
}

int
crypto_stream_chacha20_ietf_xor_ic(unsigned char *c, const unsigned char *m,
                                   unsigned long long mlen,
                                   const unsigned char *n, uint32_t ic,
                                   const unsigned char *k)
{
    return implementation->stream_ietf_xor_ic(c, m, mlen, n, ic, k);
}

int
crypto_stream_chacha20_xor(unsigned char *c, const unsigned char *m,
                           unsigned long long mlen, const unsigned char *n,
                           const unsigned char *k)
{
    return implementation->stream_xor_ic(c, m, mlen, n, 0U, k);
}

int
crypto_stream_chacha20_ietf_xor(unsigned char *c, const unsigned char *m,
                                unsigned long long mlen, const unsigned char *n,
                                const unsigned char *k)
{
    return implementation->stream_ietf_xor_ic(c, m, mlen, n, 0U, k);
}
