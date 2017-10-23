
#include <string.h>

#include "crypto_scalarmult_ed25519.h"
#include "private/curve25519_ref10.h"

int
crypto_scalarmult_ed25519(unsigned char *q, const unsigned char *n,
                          const unsigned char *p)
{
    unsigned char *t = q;
    ge_p3          Q;
    ge_p3          P;

    if (ge_is_canonical(p) != 0 || ge_has_small_order(p) != 0 ||
        ge_frombytes_negate_vartime(&P, p) != 0 ||
        ge_is_on_main_subgroup(&P) == 0) {
        return -1;
    }
    memmove(t, n, 32);
    t[0] &= 248;
    t[31] &= 63;
    t[31] |= 64;
    ge_scalarmult(&Q, t, &P);
    ge_p3_tobytes(q, &Q);
    q[31] ^= 0x80;

    return 0;
}

size_t
crypto_scalarmult_ed25519_bytes(void)
{
    return crypto_scalarmult_ed25519_BYTES;
}

size_t
crypto_scalarmult_ed25519_scalarbytes(void)
{
    return crypto_scalarmult_ed25519_SCALARBYTES;
}
