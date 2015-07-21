
#include <limits.h>
#include <string.h>

#include "crypto_aead_chacha20poly1305.h"
#include "crypto_onetimeauth_poly1305.h"
#include "crypto_stream_chacha20.h"
#include "crypto_verify_16.h"
#include "utils.h"

static unsigned char _pad0[16];

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
crypto_aead_chacha20poly1305_encrypt(unsigned char *c,
                                     unsigned long long *clen_p,
                                     const unsigned char *m,
                                     unsigned long long mlen,
                                     const unsigned char *ad,
                                     unsigned long long adlen,
                                     const unsigned char *nsec,
                                     const unsigned char *npub,
                                     const unsigned char *k)
{
    crypto_onetimeauth_poly1305_state state;
    unsigned char                     block0[64U];
    unsigned char                     slen[8U];

    (void) nsec;
/* LCOV_EXCL_START */
#ifdef ULONG_LONG_MAX
    if (mlen > ULONG_LONG_MAX - crypto_aead_chacha20poly1305_ABYTES) {
        if (clen_p != NULL) {
            *clen_p = 0ULL;
        }
        return -1;
    }
#endif
/* LCOV_EXCL_STOP */

    sodium_memzero(block0, sizeof block0);
    crypto_stream_chacha20(block0, sizeof block0, npub, k);
    crypto_onetimeauth_poly1305_init(&state, block0);
    sodium_memzero(block0, sizeof block0);

    crypto_onetimeauth_poly1305_update(&state, ad, adlen);
    _u64_le_from_ull(slen, adlen);
    crypto_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    crypto_stream_chacha20_xor_ic(c, m, mlen, npub, 1U, k);

    crypto_onetimeauth_poly1305_update(&state, c, mlen);
    _u64_le_from_ull(slen, mlen);
    crypto_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    crypto_onetimeauth_poly1305_final(&state, c + mlen);
    sodium_memzero(&state, sizeof state);

    if (clen_p != NULL) {
        *clen_p = mlen + crypto_aead_chacha20poly1305_ABYTES;
    }
    return 0;
}

int
crypto_aead_chacha20poly1305_ietf_encrypt(unsigned char *c,
                                          unsigned long long *clen_p,
                                          const unsigned char *m,
                                          unsigned long long mlen,
                                          const unsigned char *ad,
                                          unsigned long long adlen,
                                          const unsigned char *nsec,
                                          const unsigned char *npub,
                                          const unsigned char *k)
{
    crypto_onetimeauth_poly1305_state state;
    unsigned char                     block0[64U];
    unsigned char                     slen[8U];

    (void) nsec;
/* LCOV_EXCL_START */
#ifdef ULONG_LONG_MAX
    if (mlen > ULONG_LONG_MAX - crypto_aead_chacha20poly1305_ABYTES) {
        if (clen_p != NULL) {
            *clen_p = 0ULL;
        }
        return -1;
    }
#endif
/* LCOV_EXCL_STOP */

    sodium_memzero(block0, sizeof block0);
    crypto_stream_chacha20_ietf(block0, sizeof block0, npub, k);
    crypto_onetimeauth_poly1305_init(&state, block0);
    sodium_memzero(block0, sizeof block0);

    crypto_onetimeauth_poly1305_update(&state, ad, adlen);
    crypto_onetimeauth_poly1305_update(&state, _pad0, (0x10 - adlen) & 0xf);

    crypto_stream_chacha20_ietf_xor_ic(c, m, mlen, npub, 1U, k);

    crypto_onetimeauth_poly1305_update(&state, c, mlen);
    crypto_onetimeauth_poly1305_update(&state, _pad0, (0x10 - mlen) & 0xf);

    _u64_le_from_ull(slen, adlen);
    crypto_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    _u64_le_from_ull(slen, mlen);
    crypto_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    crypto_onetimeauth_poly1305_final(&state, c + mlen);
    sodium_memzero(&state, sizeof state);

    if (clen_p != NULL) {
        *clen_p = mlen + crypto_aead_chacha20poly1305_ABYTES;
    }
    return 0;
}

int
crypto_aead_chacha20poly1305_decrypt(unsigned char *m,
                                     unsigned long long *mlen_p,
                                     unsigned char *nsec,
                                     const unsigned char *c,
                                     unsigned long long clen,
                                     const unsigned char *ad,
                                     unsigned long long adlen,
                                     const unsigned char *npub,
                                     const unsigned char *k)
{
    crypto_onetimeauth_poly1305_state state;
    unsigned char                     block0[64U];
    unsigned char                     slen[8U];
    unsigned char                     mac[crypto_aead_chacha20poly1305_ABYTES];
    unsigned long long                mlen;
    int                               ret;

    (void) nsec;
    if (mlen_p != NULL) {
        *mlen_p = 0ULL;
    }
    if (clen < crypto_aead_chacha20poly1305_ABYTES) {
        return -1;
    }
    sodium_memzero(block0, sizeof block0);
    crypto_stream_chacha20(block0, sizeof block0, npub, k);
    crypto_onetimeauth_poly1305_init(&state, block0);
    sodium_memzero(block0, sizeof block0);

    crypto_onetimeauth_poly1305_update(&state, ad, adlen);
    _u64_le_from_ull(slen, adlen);
    crypto_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    mlen = clen - crypto_aead_chacha20poly1305_ABYTES;
    crypto_onetimeauth_poly1305_update(&state, c, mlen);
    _u64_le_from_ull(slen, mlen);
    crypto_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    crypto_onetimeauth_poly1305_final(&state, mac);
    sodium_memzero(&state, sizeof state);

    (void) sizeof(int[sizeof mac == 16U ? 1 : -1]);
    ret = crypto_verify_16(mac, c + mlen);
    sodium_memzero(mac, sizeof mac);
    if (ret != 0) {
        memset(m, 0, mlen);
        return -1;
    }
    crypto_stream_chacha20_xor_ic
        (m, c, mlen, npub, 1U, k);
    if (mlen_p != NULL) {
        *mlen_p = mlen;
    }
    return 0;
}

int
crypto_aead_chacha20poly1305_ietf_decrypt(unsigned char *m,
                                          unsigned long long *mlen_p,
                                          unsigned char *nsec,
                                          const unsigned char *c,
                                          unsigned long long clen,
                                          const unsigned char *ad,
                                          unsigned long long adlen,
                                          const unsigned char *npub,
                                          const unsigned char *k)
{
    crypto_onetimeauth_poly1305_state state;
    unsigned char                     block0[64U];
    unsigned char                     slen[8U];
    unsigned char                     mac[crypto_aead_chacha20poly1305_ABYTES];
    unsigned long long                mlen;
    int                               ret;

    (void) nsec;
    if (mlen_p != NULL) {
        *mlen_p = 0ULL;
    }
    if (clen < crypto_aead_chacha20poly1305_ABYTES) {
        return -1;
    }
    sodium_memzero(block0, sizeof block0);
    crypto_stream_chacha20_ietf(block0, sizeof block0, npub, k);
    crypto_onetimeauth_poly1305_init(&state, block0);
    sodium_memzero(block0, sizeof block0);

    crypto_onetimeauth_poly1305_update(&state, ad, adlen);
    crypto_onetimeauth_poly1305_update(&state, _pad0, (0x10 - adlen) & 0xf);

    mlen = clen - crypto_aead_chacha20poly1305_ABYTES;
    crypto_onetimeauth_poly1305_update(&state, c, mlen);
    crypto_onetimeauth_poly1305_update(&state, _pad0, (0x10 - mlen) & 0xf);

    _u64_le_from_ull(slen, adlen);
    crypto_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    _u64_le_from_ull(slen, mlen);
    crypto_onetimeauth_poly1305_update(&state, slen, sizeof slen);

    crypto_onetimeauth_poly1305_final(&state, mac);
    sodium_memzero(&state, sizeof state);

    (void) sizeof(int[sizeof mac == 16U ? 1 : -1]);
    ret = crypto_verify_16(mac, c + mlen);
    sodium_memzero(mac, sizeof mac);
    if (ret != 0) {
        memset(m, 0, mlen);
        return -1;
    }
    crypto_stream_chacha20_ietf_xor_ic(m, c, mlen, npub, 1U, k);
    if (mlen_p != NULL) {
        *mlen_p = mlen;
    }
    return 0;
}

size_t
crypto_aead_chacha20poly1305_keybytes(void) {
    return crypto_aead_chacha20poly1305_KEYBYTES;
}

size_t
crypto_aead_chacha20poly1305_npubbytes(void) {
    return crypto_aead_chacha20poly1305_NPUBBYTES;
}

size_t
crypto_aead_chacha20poly1305_ietf_npubbytes(void) {
    return crypto_aead_chacha20poly1305_IETF_NPUBBYTES;
}

size_t
crypto_aead_chacha20poly1305_nsecbytes(void) {
    return crypto_aead_chacha20poly1305_NSECBYTES;
}

size_t
crypto_aead_chacha20poly1305_abytes(void) {
    return crypto_aead_chacha20poly1305_ABYTES;
}
