
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "crypto_core_hchacha20.h"
#include "crypto_onetimeauth_poly1305.h"
#include "crypto_secretbox_xchacha20poly1305.h"
#include "crypto_stream_chacha20.h"
#include "private/common.h"
#include "utils.h"

#define crypto_secretbox_xchacha20poly1305_ZEROBYTES 32U

int
crypto_secretbox_xchacha20poly1305_detached(unsigned char *c,
                                            unsigned char *mac,
                                            const unsigned char *m,
                                            sodium_size_t mlen,
                                            const unsigned char *n,
                                            const unsigned char *k)
{
    crypto_onetimeauth_poly1305_state state;
    unsigned char                     block0[64U];
    unsigned char                     subkey[crypto_stream_chacha20_KEYBYTES];
    sodium_size_t                     i;
    sodium_size_t                     mlen0;

    crypto_core_hchacha20(subkey, n, k, NULL);

    if (((uintptr_t) c > (uintptr_t) m &&
         (uintptr_t) c - (uintptr_t) m < mlen) ||
        ((uintptr_t) m > (uintptr_t) c &&
         (uintptr_t) m - (uintptr_t) c < mlen)) { /* LCOV_EXCL_LINE */
        memmove(c, m, mlen);
        m = c;
    }
    memset(block0, 0U, crypto_secretbox_xchacha20poly1305_ZEROBYTES);
    COMPILER_ASSERT(64U >= crypto_secretbox_xchacha20poly1305_ZEROBYTES);
    mlen0 = mlen;
    if (mlen0 > 64U - crypto_secretbox_xchacha20poly1305_ZEROBYTES) {
        mlen0 = 64U - crypto_secretbox_xchacha20poly1305_ZEROBYTES;
    }
    for (i = 0U; i < mlen0; i++) {
        block0[i + crypto_secretbox_xchacha20poly1305_ZEROBYTES] = m[i];
    }
    crypto_stream_chacha20_xor(block0, block0,
                               mlen0 + crypto_secretbox_xchacha20poly1305_ZEROBYTES,
                               n + 16, subkey);
    COMPILER_ASSERT(crypto_secretbox_xchacha20poly1305_ZEROBYTES >=
                    crypto_onetimeauth_poly1305_KEYBYTES);
    crypto_onetimeauth_poly1305_init(&state, block0);

    for (i = 0U; i < mlen0; i++) {
        c[i] = block0[crypto_secretbox_xchacha20poly1305_ZEROBYTES + i];
    }
    sodium_memzero(block0, sizeof block0);
    if (mlen > mlen0) {
        crypto_stream_chacha20_xor_ic(c + mlen0, m + mlen0, mlen - mlen0,
                                      n + 16, 1U, subkey);
    }
    sodium_memzero(subkey, sizeof subkey);

    crypto_onetimeauth_poly1305_update(&state, c, mlen);
    crypto_onetimeauth_poly1305_final(&state, mac);
    sodium_memzero(&state, sizeof state);

    return 0;
}

int
crypto_secretbox_xchacha20poly1305_easy(unsigned char *c,
                                        const unsigned char *m,
                                        sodium_size_t mlen,
                                        const unsigned char *n,
                                        const unsigned char *k)
{
    if (mlen > crypto_secretbox_xchacha20poly1305_MESSAGEBYTES_MAX) {
        sodium_misuse();
    }
    return crypto_secretbox_xchacha20poly1305_detached
        (c + crypto_secretbox_xchacha20poly1305_MACBYTES, c, m, mlen, n, k);
}

int
crypto_secretbox_xchacha20poly1305_open_detached(unsigned char *m,
                                                 const unsigned char *c,
                                                 const unsigned char *mac,
                                                 sodium_size_t clen,
                                                 const unsigned char *n,
                                                 const unsigned char *k)
{
    unsigned char block0[64U];
    unsigned char subkey[crypto_stream_chacha20_KEYBYTES];
    sodium_size_t i;
    sodium_size_t mlen0;

    crypto_core_hchacha20(subkey, n, k, NULL);
    crypto_stream_chacha20(block0, crypto_stream_chacha20_KEYBYTES,
                           n + 16, subkey);
    if (crypto_onetimeauth_poly1305_verify(mac, c, clen, block0) != 0) {
        sodium_memzero(subkey, sizeof subkey);
        return -1;
    }
    if (m == NULL) {
        return 0;
    }
    if (((uintptr_t) c >= (uintptr_t) m &&
         (uintptr_t) c - (uintptr_t) m < clen) ||
        ((uintptr_t) m >= (uintptr_t) c &&
         (uintptr_t) m - (uintptr_t) c < clen)) { /* LCOV_EXCL_LINE */
        memmove(m, c, clen);
        c = m;
    }
    mlen0 = clen;
    if (mlen0 > 64U - crypto_secretbox_xchacha20poly1305_ZEROBYTES) {
        mlen0 = 64U - crypto_secretbox_xchacha20poly1305_ZEROBYTES;
    }
    for (i = 0U; i < mlen0; i++) {
        block0[crypto_secretbox_xchacha20poly1305_ZEROBYTES + i] = c[i];
    }
    crypto_stream_chacha20_xor(block0, block0,
                              crypto_secretbox_xchacha20poly1305_ZEROBYTES + mlen0,
                              n + 16, subkey);
    for (i = 0U; i < mlen0; i++) {
        m[i] = block0[i + crypto_secretbox_xchacha20poly1305_ZEROBYTES];
    }
    if (clen > mlen0) {
        crypto_stream_chacha20_xor_ic(m + mlen0, c + mlen0, clen - mlen0,
                                      n + 16, 1U, subkey);
    }
    sodium_memzero(subkey, sizeof subkey);

    return 0;
}

int
crypto_secretbox_xchacha20poly1305_open_easy(unsigned char *m,
                                             const unsigned char *c,
                                             sodium_size_t clen,
                                             const unsigned char *n,
                                             const unsigned char *k)
{
    if (clen < crypto_secretbox_xchacha20poly1305_MACBYTES) {
        return -1;
    }
    return crypto_secretbox_xchacha20poly1305_open_detached
        (m, c + crypto_secretbox_xchacha20poly1305_MACBYTES, c,
         clen - crypto_secretbox_xchacha20poly1305_MACBYTES, n, k);
}

size_t
crypto_secretbox_xchacha20poly1305_keybytes(void)
{
    return crypto_secretbox_xchacha20poly1305_KEYBYTES;
}

size_t
crypto_secretbox_xchacha20poly1305_noncebytes(void)
{
    return crypto_secretbox_xchacha20poly1305_NONCEBYTES;
}

size_t
crypto_secretbox_xchacha20poly1305_macbytes(void)
{
    return crypto_secretbox_xchacha20poly1305_MACBYTES;
}

size_t
crypto_secretbox_xchacha20poly1305_messagebytes_max(void)
{
    return crypto_secretbox_xchacha20poly1305_MESSAGEBYTES_MAX;
}
