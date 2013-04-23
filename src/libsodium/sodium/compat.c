
#include "crypto_auth_hmacsha256.h"
#include "crypto_auth_hmacsha512256.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#undef crypto_box_curve25519xsalsa20poly1305_ref_keypair
SODIUM_EXPORT int
crypto_box_curve25519xsalsa20poly1305_ref_keypair(unsigned char *pk,
                                                  unsigned char *sk)
{
    return crypto_box_curve25519xsalsa20poly1305_keypair(pk, sk);
}

#undef crypto_box_curve25519xsalsa20poly1305_ref_beforenm
SODIUM_EXPORT int
crypto_box_curve25519xsalsa20poly1305_ref_beforenm(unsigned char *k,
                                                   const unsigned char *pk,
                                                   const unsigned char *sk)
{
    return crypto_box_curve25519xsalsa20poly1305_beforenm(k, pk, sk);
}

#undef crypto_box_curve25519xsalsa20poly1305_ref_afternm
SODIUM_EXPORT int
crypto_box_curve25519xsalsa20poly1305_ref_afternm(unsigned char *c,
                                                  const unsigned char *m,
                                                  unsigned long long mlen,
                                                  const unsigned char *n,
                                                  const unsigned char *k)
{
    return crypto_box_curve25519xsalsa20poly1305_afternm(c, m, mlen, n, k);
}

#undef crypto_box_curve25519xsalsa20poly1305_ref_open_afternm
SODIUM_EXPORT int
crypto_box_curve25519xsalsa20poly1305_ref_open_afternm(unsigned char *m,
                                                       const unsigned char *c,
                                                       unsigned long long clen,
                                                       const unsigned char *n,
                                                       const unsigned char *k)
{
    return crypto_box_curve25519xsalsa20poly1305_open_afternm(m, c, clen, n, k);
}

#undef crypto_box_curve25519xsalsa20poly1305_ref
SODIUM_EXPORT int
crypto_box_curve25519xsalsa20poly1305_ref(unsigned char *c,
                                          const unsigned char *m,
                                          unsigned long long mlen,
                                          const unsigned char *n,
                                          const unsigned char *pk,
                                          const unsigned char *sk)
{
    return crypto_box_curve25519xsalsa20poly1305(c, m, mlen, n, pk, sk);
}

#undef crypto_box_curve25519xsalsa20poly1305_ref_open
SODIUM_EXPORT int
crypto_box_curve25519xsalsa20poly1305_ref_open(unsigned char *m,
                                               const unsigned char *c,
                                               unsigned long long clen,
                                               const unsigned char *n,
                                               const unsigned char *pk,
                                               const unsigned char *sk)
{
    return crypto_box_curve25519xsalsa20poly1305_open(m, c, clen, n, pk, sk);
}

#ifdef __cplusplus
}
#endif
