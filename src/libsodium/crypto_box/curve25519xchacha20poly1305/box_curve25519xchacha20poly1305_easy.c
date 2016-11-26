
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include "crypto_box_curve25519xchacha20poly1305.h"
#include "crypto_secretbox_xchacha20poly1305.h"
#include "utils.h"

int
crypto_box_curve25519xchacha20poly1305_detached_afternm(unsigned char *c,
                                                        unsigned char *mac,
                                                        const unsigned char *m,
                                                        unsigned long long mlen,
                                                        const unsigned char *n,
                                                        const unsigned char *k)
{
    return crypto_secretbox_xchacha20poly1305_detached(c, mac, m, mlen, n, k);
}

int
crypto_box_curve25519xchacha20poly1305_detached(unsigned char *c,
                                                unsigned char *mac,
                                                const unsigned char *m,
                                                unsigned long long mlen,
                                                const unsigned char *n,
                                                const unsigned char *pk,
                                                const unsigned char *sk)
{
    unsigned char k[crypto_box_curve25519xchacha20poly1305_BEFORENMBYTES];
    int           ret;

    (void) sizeof(int[crypto_box_curve25519xchacha20poly1305_BEFORENMBYTES
                      >= crypto_secretbox_xchacha20poly1305_KEYBYTES ?
                      1 : -1]);
    if (crypto_box_curve25519xchacha20poly1305_beforenm(k, pk, sk) != 0) {
        return -1;
    }
    ret = crypto_box_curve25519xchacha20poly1305_detached_afternm(c, mac, m,
                                                                  mlen, n, k);
    sodium_memzero(k, sizeof k);

    return ret;
}

int
crypto_box_curve25519xchacha20poly1305_easy_afternm(unsigned char *c,
                                                    const unsigned char *m,
                                                    unsigned long long mlen,
                                                    const unsigned char *n,
                                                    const unsigned char *k)
{
    if (mlen > SIZE_MAX - crypto_box_curve25519xchacha20poly1305_MACBYTES) {
        return -1;
    }
    return crypto_box_curve25519xchacha20poly1305_detached_afternm(
        c + crypto_box_curve25519xchacha20poly1305_MACBYTES, c, m, mlen, n, k);
}

int
crypto_box_curve25519xchacha20poly1305_easy(unsigned char *c,
                                            const unsigned char *m,
                                            unsigned long long mlen,
                                            const unsigned char *n,
                                            const unsigned char *pk,
                                            const unsigned char *sk)
{
    if (mlen > SIZE_MAX - crypto_box_curve25519xchacha20poly1305_MACBYTES) {
        return -1;
    }
    return crypto_box_curve25519xchacha20poly1305_detached(
        c + crypto_box_curve25519xchacha20poly1305_MACBYTES, c, m, mlen, n, pk,
        sk);
}

int
crypto_box_curve25519xchacha20poly1305_open_detached_afternm(unsigned char *m,
                                                             const unsigned char *c,
                                                             const unsigned char *mac,
                                                             unsigned long long clen,
                                                             const unsigned char *n,
                                                             const unsigned char *k)
{
    return crypto_secretbox_xchacha20poly1305_open_detached(m, c, mac, clen, n, k);
}

int
crypto_box_curve25519xchacha20poly1305_open_detached(unsigned char *m,
                                                     const unsigned char *c,
                                                     const unsigned char *mac,
                                                     unsigned long long clen,
                                                     const unsigned char *n,
                                                     const unsigned char *pk,
                                                     const unsigned char *sk)
{
    unsigned char k[crypto_box_curve25519xchacha20poly1305_BEFORENMBYTES];
    int           ret;

    if (crypto_box_curve25519xchacha20poly1305_beforenm(k, pk, sk) != 0) {
        return -1;
    }
    ret = crypto_box_curve25519xchacha20poly1305_open_detached_afternm(
        m, c, mac, clen, n, k);
    sodium_memzero(k, sizeof k);

    return ret;
}

int
crypto_box_curve25519xchacha20poly1305_open_easy_afternm(unsigned char *m,
                                                         const unsigned char *c,
                                                         unsigned long long clen,
                                                         const unsigned char *n,
                                                         const unsigned char *k)
{
    if (clen < crypto_box_curve25519xchacha20poly1305_MACBYTES) {
        return -1;
    }
    return crypto_box_curve25519xchacha20poly1305_open_detached_afternm(
        m, c + crypto_box_curve25519xchacha20poly1305_MACBYTES, c,
        clen - crypto_box_curve25519xchacha20poly1305_MACBYTES, n, k);
}

int
crypto_box_curve25519xchacha20poly1305_open_easy(unsigned char *m,
                                                 const unsigned char *c,
                                                 unsigned long long clen,
                                                 const unsigned char *n,
                                                 const unsigned char *pk,
                                                 const unsigned char *sk)
{
    if (clen < crypto_box_curve25519xchacha20poly1305_MACBYTES) {
        return -1;
    }
    return crypto_box_curve25519xchacha20poly1305_open_detached(
        m, c + crypto_box_curve25519xchacha20poly1305_MACBYTES, c,
        clen - crypto_box_curve25519xchacha20poly1305_MACBYTES, n, pk, sk);
}
