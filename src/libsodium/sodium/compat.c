
#include "crypto_auth_hmacsha256.h"
#include "crypto_auth_hmacsha512256.h"
#include "crypto_box_curve25519xsalsa20poly1305.h"
#include "crypto_hash_sha256.h"
#include "crypto_hash_sha512.h"
#include "crypto_scalarmult_curve25519.h"
#include "crypto_secretbox_xsalsa20poly1305.h"
#include "crypto_sign_ed25519.h"
#include "crypto_stream_xsalsa20.h"
#include "crypto_verify_16.h"
#include "crypto_verify_32.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef crypto_hash_sha256_ref
SODIUM_EXPORT int
crypto_hash_sha256_ref(unsigned char *out, const unsigned char *in,
                       unsigned long long inlen)
{
    return crypto_hash_sha256(out, in, inlen);
}

#undef crypto_hash_sha512_ref
SODIUM_EXPORT int
crypto_hash_sha512_ref(unsigned char *out, const unsigned char *in,
                       unsigned long long inlen)
{
    return crypto_hash_sha512(out, in, inlen);
}

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

#undef crypto_scalarmult_curve25519_ref_base
SODIUM_EXPORT int
crypto_scalarmult_curve25519_ref_base(unsigned char *q, const unsigned char *n)
{
    return crypto_scalarmult_curve25519_base(q, n);
}

#undef crypto_scalarmult_curve25519_ref
SODIUM_EXPORT int
crypto_scalarmult_curve25519_ref(unsigned char *q, const unsigned char *n,
                                 const unsigned char *p)
{
    return crypto_scalarmult_curve25519(q, n, p);
}

#undef crypto_secretbox_xsalsa20poly1305_ref
SODIUM_EXPORT int
crypto_secretbox_xsalsa20poly1305_ref(unsigned char *c,
                                      const unsigned char *m,
                                      unsigned long long mlen,
                                      const unsigned char *n,
                                      const unsigned char *k)
{
    return crypto_secretbox_xsalsa20poly1305(c, m, mlen, n, k);
}

#undef crypto_secretbox_xsalsa20poly1305_ref_open
SODIUM_EXPORT int
crypto_secretbox_xsalsa20poly1305_ref_open(unsigned char *m,
                                           const unsigned char *c,
                                           unsigned long long clen,
                                           const unsigned char *n,
                                           const unsigned char *k)
{
    return crypto_secretbox_xsalsa20poly1305_open(m, c, clen, n, k);
}

#undef crypto_sign_ed25519_ref_seed_keypair
SODIUM_EXPORT int
crypto_sign_ed25519_ref_seed_keypair(unsigned char *pk, unsigned char *sk,
                                     const unsigned char *seed)
{
    return crypto_sign_ed25519_seed_keypair(pk, sk, seed);
}

#undef crypto_sign_ed25519_ref_keypair
SODIUM_EXPORT int
crypto_sign_ed25519_ref_keypair(unsigned char *pk, unsigned char *sk)
{
    return crypto_sign_ed25519_keypair(pk, sk);
}

#undef crypto_sign_ed25519_ref
SODIUM_EXPORT int
crypto_sign_ed25519_ref(unsigned char *sm, unsigned long long *smlen,
                        const unsigned char *m, unsigned long long mlen,
                        const unsigned char *sk)
{
    return crypto_sign_ed25519(sm, smlen, m, mlen, sk);
}

#undef crypto_sign_ed25519_ref_open
SODIUM_EXPORT int
crypto_sign_ed25519_ref_open(unsigned char *m, unsigned long long *mlen,
                             const unsigned char *sm, unsigned long long smlen,
                             const unsigned char *pk)
{
    return crypto_sign_ed25519_open(m, mlen, sm, smlen, pk);
}

#undef crypto_stream_xsalsa20_ref
SODIUM_EXPORT int
crypto_stream_xsalsa20_ref(unsigned char *c, unsigned long long clen,
                           const unsigned char *n, const unsigned char *k)
{
    return crypto_stream_xsalsa20(c, clen, n, k);
}

#undef crypto_stream_xsalsa20_ref_xor
SODIUM_EXPORT int
crypto_stream_xsalsa20_ref_xor(unsigned char *c, const unsigned char *m,
                               unsigned long long mlen, const unsigned char *n,
                               const unsigned char *k)
{
    return crypto_stream_xsalsa20_xor(c, m, mlen, n, k);
}

#undef crypto_verify_16_ref
SODIUM_EXPORT int
crypto_verify_16_ref(const unsigned char *x, const unsigned char *y)
{
    return crypto_verify_16(x, y);
}

#undef crypto_verify_32_ref
SODIUM_EXPORT int
crypto_verify_32_ref(const unsigned char *x, const unsigned char *y)
{
    return crypto_verify_32(x, y);
}

#ifdef __cplusplus
}
#endif
