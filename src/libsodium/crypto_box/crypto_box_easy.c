
#include "crypto_box.h"
#include "crypto_secretbox.h"
#include "utils.h"

int
crypto_box_easy(unsigned char *c, const unsigned char *m,
                unsigned long long mlen, const unsigned char *n,
                const unsigned char *pk, const unsigned char *sk)
{
    unsigned char k[crypto_box_BEFORENMBYTES];
    int           ret;

    (void) sizeof(int[crypto_box_BEFORENMBYTES >=
                      crypto_secretbox_KEYBYTES ? 1 : -1]);
    crypto_box_beforenm(k, pk, sk);
    ret = crypto_secretbox_easy(c, m, mlen, n, k);
    sodium_memzero(k, sizeof k);

    return ret;
}

int
crypto_box_open_easy(unsigned char *m, const unsigned char *c,
                     unsigned long long clen, const unsigned char *n,
                     const unsigned char *pk, const unsigned char *sk)
{
    unsigned char k[crypto_box_BEFORENMBYTES];
    int           ret;

    crypto_box_beforenm(k, pk, sk);
    ret = crypto_secretbox_open_easy(m, c, clen, n, k);
    sodium_memzero(k, sizeof k);

    return ret;
}
