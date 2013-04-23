
#include "crypto_auth_hmacsha256.h"
#include "crypto_auth_hmacsha512256.h"
#include "export.h"

#undef crypto_auth_hmacsha256_ref
SODIUM_EXPORT int
crypto_auth_hmacsha256_ref(unsigned char *out, const unsigned char *in,
                           unsigned long long inlen, const unsigned char *k)
{
    return crypto_auth_hmacsha256(out, in, inlen, k);
}

#undef crypto_auth_hmacsha256_ref_verify
SODIUM_EXPORT int
crypto_auth_hmacsha256_ref_verify(const unsigned char *h,
                                  const unsigned char *in,
                                  unsigned long long inlen,
                                  const unsigned char *k)
{
    return crypto_auth_hmacsha256_verify(h, in, inlen, k);
}

#undef crypto_auth_hmacsha512256_ref
SODIUM_EXPORT int
crypto_auth_hmacsha512256_ref(unsigned char *out, const unsigned char *in,
                              unsigned long long inlen, const unsigned char *k)
{
    return crypto_auth_hmacsha512256(out, in, inlen, k);
}

#undef crypto_auth_hmacsha512256_ref_verify
SODIUM_EXPORT int
crypto_auth_hmacsha512256_ref_verify(const unsigned char *h,
                                     const unsigned char *in,
                                     unsigned long long inlen,
                                     const unsigned char *k)
{
    return crypto_auth_hmacsha512256_verify(h, in, inlen, k);
}
