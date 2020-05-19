
#include <errno.h>
#include <stdlib.h>

#include "crypto_aead_aegis128l.h"
#include "private/common.h"
#include "randombytes.h"

size_t
crypto_aead_aegis128l_keybytes(void)
{
    return crypto_aead_aegis128l_KEYBYTES;
}

size_t
crypto_aead_aegis128l_nsecbytes(void)
{
    return crypto_aead_aegis128l_NSECBYTES;
}

size_t
crypto_aead_aegis128l_npubbytes(void)
{
    return crypto_aead_aegis128l_NPUBBYTES;
}

size_t
crypto_aead_aegis128l_abytes(void)
{
    return crypto_aead_aegis128l_ABYTES;
}

size_t
crypto_aead_aegis128l_messagebytes_max(void)
{
    return crypto_aead_aegis128l_MESSAGEBYTES_MAX;
}

void
crypto_aead_aegis128l_keygen(unsigned char k[crypto_aead_aegis128l_KEYBYTES])
{
    randombytes_buf(k, crypto_aead_aegis128l_KEYBYTES);
}

#if !((defined(HAVE_TMMINTRIN_H) && defined(HAVE_WMMINTRIN_H)) || \
      defined(HAVE_ARMCRYPTO))

#ifndef ENOSYS
# define ENOSYS ENXIO
#endif

int
crypto_aead_aegis128l_encrypt_detached(unsigned char *c, unsigned char *mac,
                                       unsigned long long *maclen_p, const unsigned char *m,
                                       unsigned long long mlen, const unsigned char *ad,
                                       unsigned long long adlen, const unsigned char *nsec,
                                       const unsigned char *npub, const unsigned char *k)
{
    errno = ENOSYS;
    return -1;
}

int
crypto_aead_aegis128l_encrypt(unsigned char *c, unsigned long long *clen_p, const unsigned char *m,
                              unsigned long long mlen, const unsigned char *ad,
                              unsigned long long adlen, const unsigned char *nsec,
                              const unsigned char *npub, const unsigned char *k)
{
    errno = ENOSYS;
    return -1;
}

int
crypto_aead_aegis128l_decrypt_detached(unsigned char *m, unsigned char *nsec, const unsigned char *c,
                                       unsigned long long clen, const unsigned char *mac,
                                       const unsigned char *ad, unsigned long long adlen,
                                       const unsigned char *npub, const unsigned char *k)
{
    errno = ENOSYS;
    return -1;
}

int
crypto_aead_aegis128l_decrypt(unsigned char *m, unsigned long long *mlen_p, unsigned char *nsec,
                              const unsigned char *c, unsigned long long clen,
                              const unsigned char *ad, unsigned long long adlen,
                              const unsigned char *npub, const unsigned char *k)
{
    errno = ENOSYS;
    return -1;
}

int
crypto_aead_aegis128l_is_available(void)
{
    return 0;
}

#endif
