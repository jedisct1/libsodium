
#include "crypto_onetimeauth.h"

size_t
crypto_onetimeauth_bytes(void)
{
    return crypto_onetimeauth_BYTES;
}

size_t
crypto_onetimeauth_keybytes(void)
{
    return crypto_onetimeauth_KEYBYTES;
}

const char *
crypto_onetimeauth_primitive(void)
{
    return crypto_onetimeauth_PRIMITIVE;
}

int
crypto_onetimeauth(unsigned char *out, const unsigned char *in,
                   unsigned long long inlen, const unsigned char *k)
{
    return crypto_onetimeauth_poly1305(out, in, inlen, k);
}

int
crypto_onetimeauth_verify(const unsigned char *h, const unsigned char *in,
                          unsigned long long inlen, const unsigned char *k)
{
    return crypto_onetimeauth_poly1305_verify(h, in, inlen, k);
}
