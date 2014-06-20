
#include <string.h>

#include "crypto_onetimeauth_poly1305.h"
#include "crypto_secretbox_chacha20poly1305.h"
#include "crypto_stream_chacha20.h"
#include "crypto_verify_16.h"
#include "utils.h"

static inline void
_u64_le_from_ull(unsigned char out[8U], unsigned long long x)
{
    out[0] = (unsigned char) (x & 0xff); x >>= 8;
    out[1] = (unsigned char) (x & 0xff); x >>= 8;
    out[2] = (unsigned char) (x & 0xff); x >>= 8;
    out[3] = (unsigned char) (x & 0xff); x >>= 8;
    out[4] = (unsigned char) (x & 0xff); x >>= 8;
    out[5] = (unsigned char) (x & 0xff); x >>= 8;
    out[6] = (unsigned char) (x & 0xff); x >>= 8;
    out[7] = (unsigned char) (x & 0xff);
}

int
crypto_secretbox_chacha20poly1305_ad(unsigned char *c,
                                     const unsigned char *m,
                                     unsigned long long mlen,
                                     const unsigned char *ad,
                                     unsigned long long adlen,
                                     const unsigned char *n,
                                     const unsigned char *k)
{
    crypto_onetimeauth_poly1305_state state;
    unsigned char                     block0[64U];
    unsigned char                     slen[8U];

    crypto_stream_chacha20_xor_ic
        (c + crypto_secretbox_chacha20poly1305_ZEROBYTES, m, mlen, n, 1U, k);

    crypto_stream_chacha20(block0, sizeof block0, n, k);
    crypto_onetimeauth_poly1305_init(&state, block0);
    sodium_memzero(block0, sizeof block0);

    crypto_onetimeauth_poly1305_update(&state, ad, adlen);
    _u64_le_from_ull(slen, adlen);
    crypto_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    crypto_onetimeauth_poly1305_update
        (&state, c + crypto_secretbox_chacha20poly1305_ZEROBYTES, mlen);
    _u64_le_from_ull(slen, mlen);
    crypto_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    crypto_onetimeauth_poly1305_final(&state, c);

    return 0;
}

int
crypto_secretbox_chacha20poly1305(unsigned char *c,
                                  const unsigned char *m,
                                  unsigned long long mlen,
                                  const unsigned char *n,
                                  const unsigned char *k)
{
    return crypto_secretbox_chacha20poly1305_ad(c, m, mlen, NULL, 0ULL, n, k);
}

int
crypto_secretbox_chacha20poly1305_ad_open(unsigned char *m,
                                          const unsigned char *c,
                                          unsigned long long clen,
                                          const unsigned char *ad,
                                          unsigned long long adlen,
                                          const unsigned char *n,
                                          const unsigned char *k)
{
    crypto_onetimeauth_poly1305_state state;
    unsigned char                     block0[64U];
    unsigned char                     slen[8U];
    unsigned char                     mac[crypto_secretbox_chacha20poly1305_MACBYTES];
    int                               ret;

    if (clen < crypto_secretbox_chacha20poly1305_ZEROBYTES) {
        return -1;
    }

    crypto_stream_chacha20(block0, sizeof block0, n, k);
    crypto_onetimeauth_poly1305_init(&state, block0);
    sodium_memzero(block0, sizeof block0);

    crypto_onetimeauth_poly1305_update(&state, ad, adlen);
    _u64_le_from_ull(slen, adlen);
    crypto_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    crypto_onetimeauth_poly1305_update
        (&state, c + crypto_secretbox_chacha20poly1305_ZEROBYTES,
         clen - crypto_secretbox_chacha20poly1305_ZEROBYTES);
    _u64_le_from_ull(slen, clen - crypto_secretbox_chacha20poly1305_ZEROBYTES);
    crypto_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    crypto_onetimeauth_poly1305_final(&state, mac);

    ret = crypto_verify_16(mac, c);
    sodium_memzero(mac, sizeof mac);
    if (ret != 0) {
        memset(m, 0, clen - crypto_secretbox_chacha20poly1305_ZEROBYTES);
        return -1;
    }
    crypto_stream_chacha20_xor_ic
        (m, c + crypto_secretbox_chacha20poly1305_ZEROBYTES,
         clen - crypto_secretbox_chacha20poly1305_ZEROBYTES, n, 1U, k);

    return 0;
}

int
crypto_secretbox_chacha20poly1305_open(unsigned char *m,
                                       const unsigned char *c,
                                       unsigned long long clen,
                                       const unsigned char *n,
                                       const unsigned char *k)
{
    return crypto_secretbox_chacha20poly1305_ad_open(m, c, clen,
                                                     NULL, 0ULL, n, k);
}

size_t
crypto_secretbox_chacha20poly1305_keybytes(void) {
    return crypto_secretbox_chacha20poly1305_KEYBYTES;
}

size_t
crypto_secretbox_chacha20poly1305_noncebytes(void) {
    return crypto_secretbox_chacha20poly1305_NONCEBYTES;
}

size_t
crypto_secretbox_chacha20poly1305_zerobytes(void) {
    return crypto_secretbox_chacha20poly1305_ZEROBYTES;
}

size_t
crypto_secretbox_chacha20poly1305_boxzerobytes(void) {
    return crypto_secretbox_chacha20poly1305_BOXZEROBYTES;
}

size_t
crypto_secretbox_chacha20poly1305_macbytes(void) {
    return crypto_secretbox_chacha20poly1305_MACBYTES;
}
