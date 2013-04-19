
#include "crypto_scalarmult.h"

int
crypto_scalarmult_base(unsigned char *q, const unsigned char *n)
{
    return crypto_scalarmult_curve25519_base(q, n);
}

int
crypto_scalarmult(unsigned char *q, const unsigned char *n,
                  const unsigned char *p)
{
    return crypto_scalarmult_curve25519(q, n, p);
}

