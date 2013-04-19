
#include "crypto_generichash.h"

int
crypto_generichash(unsigned char *out, const unsigned char *in,
                   const unsigned char *key,
                   size_t outlen, unsigned long long inlen, size_t keylen)
{
    return crypto_generichash_blake2b(out, in, key, outlen, inlen, keylen);
}

    



