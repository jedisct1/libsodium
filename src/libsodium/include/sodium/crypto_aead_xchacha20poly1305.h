#ifndef crypto_aead_xchacha20poly1305_H
#define crypto_aead_xchacha20poly1305_H

#include <stddef.h>
#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_aead_xchacha20poly1305_KEYBYTES 32U
SODIUM_EXPORT
size_t crypto_aead_xchacha20poly1305_keybytes(void);

#define crypto_aead_xchacha20poly1305_NSECBYTES 0U
SODIUM_EXPORT
size_t crypto_aead_xchacha20poly1305_nsecbytes(void);

#define crypto_aead_xchacha20poly1305_NPUBBYTES 24U
SODIUM_EXPORT
size_t crypto_aead_xchacha20poly1305_npubbytes(void);

#define crypto_aead_xchacha20poly1305_ABYTES 16U
SODIUM_EXPORT
size_t crypto_aead_xchacha20poly1305_abytes(void);

#define crypto_aead_xchacha20poly1305_ietf_KEYBYTES 32U
SODIUM_EXPORT
size_t crypto_aead_xchacha20poly1305_ietf_keybytes(void);

#define crypto_aead_xchacha20poly1305_ietf_NSECBYTES 0U
SODIUM_EXPORT
size_t crypto_aead_xchacha20poly1305_ietf_nsecbytes(void);

#define crypto_aead_xchacha20poly1305_ietf_NPUBBYTES 24U
SODIUM_EXPORT
size_t crypto_aead_xchacha20poly1305_ietf_npubbytes(void);

#define crypto_aead_xchacha20poly1305_ietf_ABYTES 16U
SODIUM_EXPORT
size_t crypto_aead_xchacha20poly1305_ietf_abytes(void);

SODIUM_EXPORT
int crypto_aead_xchacha20poly1305_encrypt(unsigned char *c,
                                          unsigned long long *clen_p,
                                          const unsigned char *m,
                                          unsigned long long mlen,
                                          const unsigned char *ad,
                                          unsigned long long adlen,
                                          const unsigned char *nsec,
                                          const unsigned char *npub,
                                          const unsigned char *k);

SODIUM_EXPORT
int crypto_aead_xchacha20poly1305_decrypt(unsigned char *m,
                                          unsigned long long *mlen_p,
                                          unsigned char *nsec,
                                          const unsigned char *c,
                                          unsigned long long clen,
                                          const unsigned char *ad,
                                          unsigned long long adlen,
                                          const unsigned char *npub,
                                          const unsigned char *k)
            __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_aead_xchacha20poly1305_encrypt_detached(unsigned char *c,
                                                   unsigned char *mac,
                                                   unsigned long long *maclen_p,
                                                   const unsigned char *m,
                                                   unsigned long long mlen,
                                                   const unsigned char *ad,
                                                   unsigned long long adlen,
                                                   const unsigned char *nsec,
                                                   const unsigned char *npub,
                                                   const unsigned char *k);

SODIUM_EXPORT
int crypto_aead_xchacha20poly1305_decrypt_detached(unsigned char *m,
                                                   unsigned char *nsec,
                                                   const unsigned char *c,
                                                   unsigned long long clen,
                                                   const unsigned char *mac,
                                                   const unsigned char *ad,
                                                   unsigned long long adlen,
                                                   const unsigned char *npub,
                                                   const unsigned char *k)
        __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_aead_xchacha20poly1305_ietf_encrypt(unsigned char *c,
                                          unsigned long long *clen_p,
                                          const unsigned char *m,
                                          unsigned long long mlen,
                                          const unsigned char *ad,
                                          unsigned long long adlen,
                                          const unsigned char *nsec,
                                          const unsigned char *npub,
                                          const unsigned char *k);

SODIUM_EXPORT
int crypto_aead_xchacha20poly1305_ietf_decrypt(unsigned char *m,
                                          unsigned long long *mlen_p,
                                          unsigned char *nsec,
                                          const unsigned char *c,
                                          unsigned long long clen,
                                          const unsigned char *ad,
                                          unsigned long long adlen,
                                          const unsigned char *npub,
                                          const unsigned char *k)
            __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_aead_xchacha20poly1305_ietf_encrypt_detached(unsigned char *c,
                                                   unsigned char *mac,
                                                   unsigned long long *maclen_p,
                                                   const unsigned char *m,
                                                   unsigned long long mlen,
                                                   const unsigned char *ad,
                                                   unsigned long long adlen,
                                                   const unsigned char *nsec,
                                                   const unsigned char *npub,
                                                   const unsigned char *k);

SODIUM_EXPORT
int crypto_aead_xchacha20poly1305_ietf_decrypt_detached(unsigned char *m,
                                                   unsigned char *nsec,
                                                   const unsigned char *c,
                                                   unsigned long long clen,
                                                   const unsigned char *mac,
                                                   const unsigned char *ad,
                                                   unsigned long long adlen,
                                                   const unsigned char *npub,
                                                   const unsigned char *k)
        __attribute__ ((warn_unused_result));

/* Aliases */

#define crypto_aead_xchacha20poly1305_IETF_KEYBYTES  crypto_aead_xchacha20poly1305_ietf_KEYBYTES
#define crypto_aead_xchacha20poly1305_IETF_NSECBYTES crypto_aead_xchacha20poly1305_ietf_NSECBYTES
#define crypto_aead_xchacha20poly1305_IETF_NPUBBYTES crypto_aead_xchacha20poly1305_ietf_NPUBBYTES
#define crypto_aead_xchacha20poly1305_IETF_ABYTES    crypto_aead_xchacha20poly1305_ietf_ABYTES

#ifdef __cplusplus
}
#endif

#endif
