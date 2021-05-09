
#include "crypto_scalarmult_curve25519.h"
#include "private/implementations.h"
#include "scalarmult_curve25519.h"
#include "runtime.h"

#ifdef HAVE_AVX_ASM
# include "sandy2x/curve25519_sandy2x.h"
#endif
#include "ref10/x25519_ref10.h"
static const crypto_scalarmult_curve25519_implementation *implementation =
    &crypto_scalarmult_curve25519_ref10_implementation;

static int
_crypto_scalarmult_curve25519(unsigned char *q, const unsigned char *n,
                              const unsigned char *p, const int clamp)
{
    size_t                 i;
    volatile unsigned char d = 0;

    if (implementation->mult(q, n, p, clamp) != 0) {
        return -1; /* LCOV_EXCL_LINE */
    }
    for (i = 0; i < crypto_scalarmult_curve25519_BYTES; i++) {
        d |= q[i];
    }
    return -(1 & ((d - 1) >> 8));
}

int
crypto_scalarmult_curve25519(unsigned char *q, const unsigned char *n,
                             const unsigned char *p)
{
    return _crypto_scalarmult_curve25519(q, n, p, 1);
}

int
crypto_scalarmult_curve25519_noclamp(unsigned char *q, const unsigned char *n,
                                     const unsigned char *p)
{
    return _crypto_scalarmult_curve25519(q, n, p, 0);
}

int
crypto_scalarmult_curve25519_base(unsigned char *q, const unsigned char *n)
{
    return crypto_scalarmult_curve25519_ref10_implementation
        .mult_base(q, n, 1);
}

int
crypto_scalarmult_curve25519_base_noclamp(unsigned char *q, const unsigned char *n)
{
    return crypto_scalarmult_curve25519_ref10_implementation
        .mult_base(q, n, 0);
}

size_t
crypto_scalarmult_curve25519_bytes(void)
{
    return crypto_scalarmult_curve25519_BYTES;
}

size_t
crypto_scalarmult_curve25519_scalarbytes(void)
{
    return crypto_scalarmult_curve25519_SCALARBYTES;
}

int
_crypto_scalarmult_curve25519_pick_best_implementation(void)
{
    implementation = &crypto_scalarmult_curve25519_ref10_implementation;

#ifdef HAVE_AVX_ASM
    if (sodium_runtime_has_avx()) {
        implementation = &crypto_scalarmult_curve25519_sandy2x_implementation;
    }
#endif
    return 0;
}
