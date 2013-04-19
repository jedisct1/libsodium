
#include "crypto_box.h"

int
crypto_box_keypair(unsigned char *pk, unsigned char *sk)
{
    return crypto_box_curve25519xsalsa20poly1305_keypair(pk, sk);
}

int
crypto_box_beforenm(unsigned char *k, const unsigned char *pk,
                    const unsigned char *sk)
{
    return crypto_box_curve25519xsalsa20poly1305_beforenm(k, pk, sk);
}

int
crypto_box_afternm(unsigned char *c, const unsigned char *m,
                   unsigned long long mlen, const unsigned char *n,
                   const unsigned char *k)
{
    return crypto_box_curve25519xsalsa20poly1305_afternm(c, m, mlen, n, k);
}

int
crypto_box_open_afternm(unsigned char *m, const unsigned char *c,
                        unsigned long long clen, const unsigned char *n,
                        const unsigned char *k)
{
    return crypto_box_curve25519xsalsa20poly1305_open_afternm(m, c, clen, m, k);
}

int
crypto_box(unsigned char *c, const unsigned char *m,
           unsigned long long mlen, const unsigned char *n,
           const unsigned char *pk, const unsigned char *sk)
{
    return crypto_box_curve25519xsalsa20poly1305(c, m, mlen, n, pk, sk);
}

int
crypto_box_open(unsigned char *m, const unsigned char *c,
                unsigned long long clen, const unsigned char *n,
                const unsigned char *pk, const unsigned char *sk)
{
    return crypto_box_curve25519xsalsa20poly1305_open(m, c, clen, n, pk, sk);
}
