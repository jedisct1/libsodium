
#include <stddef.h>
#include <stdint.h>

#ifndef HAVE_TI_MODE

#include "../scalarmult_curve25519.h"
#include "private/curve25519_ref10.h"
#include "utils.h"
#include "x25519_ref10.h"

static int
crypto_scalarmult_curve25519_ref10(unsigned char *q,
                                   const unsigned char *n,
                                   const unsigned char *p)
{
    unsigned char *t = q;
    unsigned int   i;
    fe             x1;
    fe             x2;
    fe             z2;
    fe             x3;
    fe             z3;
    fe             tmp0;
    fe             tmp1;
    int            pos;
    unsigned int   swap;
    unsigned int   b;

    for (i = 0; i < 32; i++) {
        t[i] = n[i];
    }
    t[0] &= 248;
    t[31] &= 127;
    t[31] |= 64;
    fe_frombytes(x1, p);
    fe_1(x2);
    fe_0(z2);
    fe_copy(x3, x1);
    fe_1(z3);

    swap = 0;
    for (pos = 254; pos >= 0; --pos) {
        b = t[pos / 8] >> (pos & 7);
        b &= 1;
        swap ^= b;
        fe_cswap(x2, x3, swap);
        fe_cswap(z2, z3, swap);
        swap = b;
        fe_sub(tmp0, x3, z3);
        fe_sub(tmp1, x2, z2);
        fe_add(x2, x2, z2);
        fe_add(z2, x3, z3);
        fe_mul(z3, tmp0, x2);
        fe_mul(z2, z2, tmp1);
        fe_sq(tmp0, tmp1);
        fe_sq(tmp1, x2);
        fe_add(x3, z3, z2);
        fe_sub(z2, z3, z2);
        fe_mul(x2, tmp1, tmp0);
        fe_sub(tmp1, tmp1, tmp0);
        fe_sq(z2, z2);
        fe_scalar_product(z3, tmp1, 121666);
        fe_sq(x3, x3);
        fe_add(tmp0, tmp0, z3);
        fe_mul(z3, x1, z2);
        fe_mul(z2, tmp1, tmp0);
    }
    fe_cswap(x2, x3, swap);
    fe_cswap(z2, z3, swap);

    fe_invert(z2, z2);
    fe_mul(x2, x2, z2);
    fe_tobytes(q, x2);

    return 0;
}

static void
edwards_to_montgomery(fe montgomeryX, const fe edwardsY, const fe edwardsZ)
{
    fe tempX;
    fe tempZ;

    fe_add(tempX, edwardsZ, edwardsY);
    fe_sub(tempZ, edwardsZ, edwardsY);
    fe_invert(tempZ, tempZ);
    fe_mul(montgomeryX, tempX, tempZ);
}

static int
crypto_scalarmult_curve25519_ref10_base(unsigned char *q,
                                        const unsigned char *n)
{
    unsigned char *t = q;
    ge_p3          A;
    fe             pk;
    unsigned int   i;

    for (i = 0; i < 32; i++) {
        t[i] = n[i];
    }
    t[0] &= 248;
    t[31] &= 127;
    t[31] |= 64;
    ge_scalarmult_base(&A, t);
    edwards_to_montgomery(pk, A.Y, A.Z);
    fe_tobytes(q, pk);

    return 0;
}

struct crypto_scalarmult_curve25519_implementation
    crypto_scalarmult_curve25519_ref10_implementation = {
        SODIUM_C99(.mult =) crypto_scalarmult_curve25519_ref10,
        SODIUM_C99(.mult_base =) crypto_scalarmult_curve25519_ref10_base
    };

#endif
