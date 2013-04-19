
#include "crypto_hash.h"

int
crypto_hash(unsigned char *out, const unsigned char *in,
            unsigned long long inlen)
{
    return crypto_hash_sha512(out, in, inlen);
}
