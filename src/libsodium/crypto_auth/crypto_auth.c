
#include "crypto_auth.h"

int crypto_auth(unsigned char *out, const unsigned char *in,
                unsigned long long inlen, const unsigned char *k)
{
    return crypto_auth_hmacsha512256(out, in, inlen, k);
}

int crypto_auth_verify(const unsigned char *h, const unsigned char *in,
                       unsigned long long inlen,const unsigned char *k)
{
    return crypto_auth_hmacsha512256_verify(h, in, inlen, k);
}
