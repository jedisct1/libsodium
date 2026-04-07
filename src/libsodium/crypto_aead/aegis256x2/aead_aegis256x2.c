
#include <errno.h>
#include <stdlib.h>

#include "core.h"
#include "crypto_aead_aegis256x2.h"
#include "private/common.h"
#include "private/implementations.h"
#include "randombytes.h"
#include "runtime.h"

#include "aegis256x2_soft.h"

#if defined(HAVE_AVXINTRIN_H) && defined(HAVE_WMMINTRIN_H)
#include "aegis256x2_aesni.h"
#endif

#if defined(HAVE_AVX2INTRIN_H) && defined(HAVE_VAESINTRIN_H)
#include "aegis256x2_avx2.h"
#endif

static const aegis256x2_implementation *implementation = &aegis256x2_soft_implementation;

size_t
crypto_aead_aegis256x2_keybytes(void)
{
    return crypto_aead_aegis256x2_KEYBYTES;
}

size_t
crypto_aead_aegis256x2_nsecbytes(void)
{
    return crypto_aead_aegis256x2_NSECBYTES;
}

size_t
crypto_aead_aegis256x2_npubbytes(void)
{
    return crypto_aead_aegis256x2_NPUBBYTES;
}

size_t
crypto_aead_aegis256x2_abytes(void)
{
    return crypto_aead_aegis256x2_ABYTES;
}

size_t
crypto_aead_aegis256x2_messagebytes_max(void)
{
    return crypto_aead_aegis256x2_MESSAGEBYTES_MAX;
}

void
crypto_aead_aegis256x2_keygen(unsigned char k[crypto_aead_aegis256x2_KEYBYTES])
{
    randombytes_buf(k, crypto_aead_aegis256x2_KEYBYTES);
}

int
crypto_aead_aegis256x2_encrypt(unsigned char *c, unsigned long long *clen_p, const unsigned char *m,
                              unsigned long long mlen, const unsigned char *ad,
                              unsigned long long adlen, const unsigned char *nsec,
                              const unsigned char *npub, const unsigned char *k)
{
    unsigned long long clen = 0ULL;
    int                ret;

    ret = crypto_aead_aegis256x2_encrypt_detached(c, c + mlen, NULL, m, mlen, ad, adlen, nsec, npub,
                                                 k);
    if (clen_p != NULL) {
        if (ret == 0) {
            clen = mlen + crypto_aead_aegis256x2_ABYTES;
        }
        *clen_p = clen;
    }
    return ret;
}

int
crypto_aead_aegis256x2_decrypt(unsigned char *m, unsigned long long *mlen_p, unsigned char *nsec,
                              const unsigned char *c, unsigned long long clen,
                              const unsigned char *ad, unsigned long long adlen,
                              const unsigned char *npub, const unsigned char *k)
{
    unsigned long long mlen = 0ULL;
    int                ret  = -1;

    if (clen >= crypto_aead_aegis256x2_ABYTES) {
        ret = crypto_aead_aegis256x2_decrypt_detached(
            m, nsec, c, clen - crypto_aead_aegis256x2_ABYTES,
            c + clen - crypto_aead_aegis256x2_ABYTES, ad, adlen, npub, k);
    }
    if (mlen_p != NULL) {
        if (ret == 0) {
            mlen = clen - crypto_aead_aegis256x2_ABYTES;
        }
        *mlen_p = mlen;
    }
    return ret;
}

int
crypto_aead_aegis256x2_encrypt_detached(unsigned char *c, unsigned char *mac,
                                       unsigned long long *maclen_p, const unsigned char *m,
                                       unsigned long long mlen, const unsigned char *ad,
                                       unsigned long long adlen, const unsigned char *nsec,
                                       const unsigned char *npub, const unsigned char *k)
{
    const size_t maclen = crypto_aead_aegis256x2_ABYTES;

    (void) nsec;
    if (maclen_p != NULL) {
        *maclen_p = maclen;
    }
    if (mlen > crypto_aead_aegis256x2_MESSAGEBYTES_MAX ||
        adlen > crypto_aead_aegis256x2_MESSAGEBYTES_MAX) {
        sodium_misuse(); /* LCOV_EXCL_LINE */
    }
    return implementation->encrypt_detached(c, mac, maclen, m, (size_t) mlen, ad, (size_t) adlen,
                                            npub, k);
}

int
crypto_aead_aegis256x2_decrypt_detached(unsigned char *m, unsigned char *nsec,
                                       const unsigned char *c, unsigned long long clen,
                                       const unsigned char *mac, const unsigned char *ad,
                                       unsigned long long adlen, const unsigned char *npub,
                                       const unsigned char *k)
{
    const size_t maclen = crypto_aead_aegis256x2_ABYTES;

    (void) nsec;
    if (clen > crypto_aead_aegis256x2_MESSAGEBYTES_MAX ||
        adlen > crypto_aead_aegis256x2_MESSAGEBYTES_MAX) {
        return -1; /* LCOV_EXCL_LINE */
    }
    return implementation->decrypt_detached(m, c, (size_t) clen, mac, maclen, ad, (size_t) adlen,
                                            npub, k);
}

int
_crypto_aead_aegis256x2_pick_best_implementation(void)
{
    implementation = &aegis256x2_soft_implementation;

#if defined(HAVE_AVX2INTRIN_H) && defined(HAVE_VAESINTRIN_H)
    if (sodium_runtime_has_vaes() & sodium_runtime_has_avx2()) {
        implementation = &aegis256x2_avx2_implementation;
        return 0;
    }
#endif
#if defined(HAVE_AVXINTRIN_H) && defined(HAVE_WMMINTRIN_H)
    if (sodium_runtime_has_aesni() & sodium_runtime_has_avx()) {
        implementation = &aegis256x2_aesni_implementation;
        return 0;
    }
#endif
    return 0; /* LCOV_EXCL_LINE */
}
