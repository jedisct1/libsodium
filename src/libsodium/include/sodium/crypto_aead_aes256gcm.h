#ifndef crypto_aead_aes256gcm_H
#define crypto_aead_aes256gcm_H

/*
 * WARNING: Despite being the most popular AEAD construction due to its
 * use in TLS, safely using AES-GCM in a different context is tricky.
 *
 * No more than ~ 350 GB of input data should be encrypted with a given key.
 * This is for ~ 16 KB messages -- Actual figures vary according to
 * message sizes.
 *
 * In addition, nonces are short and repeated nonces would totally destroy
 * the security of this scheme.
 *
 * Nonces should thus come from atomic counters, which can be difficult to
 * set up in a distributed environment.
 *
 * Unless you absolutely need AES-GCM, use crypto_aead_xchacha20poly1305_ietf_*()
 * instead. It doesn't have any of these limitations.
 * Or, if you don't need to authenticate additional data, just stick to
 * crypto_secretbox().
 */

#include <stddef.h>
#include "export.h"

#ifdef __cplusplus
# if defined(__GNUC__) && !defined(SODIUM_LIBRARY_SIZE_T)
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

SODIUM_EXPORT
int crypto_aead_aes256gcm_is_available(void);

#define crypto_aead_aes256gcm_KEYBYTES  32U
SODIUM_EXPORT
size_t crypto_aead_aes256gcm_keybytes(void);

#define crypto_aead_aes256gcm_NSECBYTES 0U
SODIUM_EXPORT
size_t crypto_aead_aes256gcm_nsecbytes(void);

#define crypto_aead_aes256gcm_NPUBBYTES 12U
SODIUM_EXPORT
size_t crypto_aead_aes256gcm_npubbytes(void);

#define crypto_aead_aes256gcm_ABYTES    16U
SODIUM_EXPORT
size_t crypto_aead_aes256gcm_abytes(void);

#define crypto_aead_aes256gcm_MESSAGEBYTES_MAX \
    SODIUM_MIN(SODIUM_SIZE_MAX - crypto_aead_aes256gcm_ABYTES, \
               (16ULL * ((1ULL << 32) - 2ULL)) - crypto_aead_aes256gcm_ABYTES)
SODIUM_EXPORT
size_t crypto_aead_aes256gcm_messagebytes_max(void);

typedef CRYPTO_ALIGN(16) unsigned char crypto_aead_aes256gcm_state[512];

SODIUM_EXPORT
size_t crypto_aead_aes256gcm_statebytes(void);

SODIUM_EXPORT
int crypto_aead_aes256gcm_encrypt(unsigned char *c,
                                  sodium_size_t *clen_p,
                                  const unsigned char *m,
                                  sodium_size_t mlen,
                                  const unsigned char *ad,
                                  sodium_size_t adlen,
                                  const unsigned char *nsec,
                                  const unsigned char *npub,
                                  const unsigned char *k);

SODIUM_EXPORT
int crypto_aead_aes256gcm_decrypt(unsigned char *m,
                                  sodium_size_t *mlen_p,
                                  unsigned char *nsec,
                                  const unsigned char *c,
                                  sodium_size_t clen,
                                  const unsigned char *ad,
                                  sodium_size_t adlen,
                                  const unsigned char *npub,
                                  const unsigned char *k)
            __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_aead_aes256gcm_encrypt_detached(unsigned char *c,
                                           unsigned char *mac,
                                           sodium_size_t *maclen_p,
                                           const unsigned char *m,
                                           sodium_size_t mlen,
                                           const unsigned char *ad,
                                           sodium_size_t adlen,
                                           const unsigned char *nsec,
                                           const unsigned char *npub,
                                           const unsigned char *k);

SODIUM_EXPORT
int crypto_aead_aes256gcm_decrypt_detached(unsigned char *m,
                                           unsigned char *nsec,
                                           const unsigned char *c,
                                           sodium_size_t clen,
                                           const unsigned char *mac,
                                           const unsigned char *ad,
                                           sodium_size_t adlen,
                                           const unsigned char *npub,
                                           const unsigned char *k)
        __attribute__ ((warn_unused_result));

/* -- Precomputation interface -- */

SODIUM_EXPORT
int crypto_aead_aes256gcm_beforenm(crypto_aead_aes256gcm_state *ctx_,
                                   const unsigned char *k);

SODIUM_EXPORT
int crypto_aead_aes256gcm_encrypt_afternm(unsigned char *c,
                                          sodium_size_t *clen_p,
                                          const unsigned char *m,
                                          sodium_size_t mlen,
                                          const unsigned char *ad,
                                          sodium_size_t adlen,
                                          const unsigned char *nsec,
                                          const unsigned char *npub,
                                          const crypto_aead_aes256gcm_state *ctx_);

SODIUM_EXPORT
int crypto_aead_aes256gcm_decrypt_afternm(unsigned char *m,
                                          sodium_size_t *mlen_p,
                                          unsigned char *nsec,
                                          const unsigned char *c,
                                          sodium_size_t clen,
                                          const unsigned char *ad,
                                          sodium_size_t adlen,
                                          const unsigned char *npub,
                                          const crypto_aead_aes256gcm_state *ctx_)
            __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_aead_aes256gcm_encrypt_detached_afternm(unsigned char *c,
                                                   unsigned char *mac,
                                                   sodium_size_t *maclen_p,
                                                   const unsigned char *m,
                                                   sodium_size_t mlen,
                                                   const unsigned char *ad,
                                                   sodium_size_t adlen,
                                                   const unsigned char *nsec,
                                                   const unsigned char *npub,
                                                   const crypto_aead_aes256gcm_state *ctx_);

SODIUM_EXPORT
int crypto_aead_aes256gcm_decrypt_detached_afternm(unsigned char *m,
                                                   unsigned char *nsec,
                                                   const unsigned char *c,
                                                   sodium_size_t clen,
                                                   const unsigned char *mac,
                                                   const unsigned char *ad,
                                                   sodium_size_t adlen,
                                                   const unsigned char *npub,
                                                   const crypto_aead_aes256gcm_state *ctx_)
        __attribute__ ((warn_unused_result));

SODIUM_EXPORT
void crypto_aead_aes256gcm_keygen(unsigned char k[crypto_aead_aes256gcm_KEYBYTES]);

#ifdef __cplusplus
}
#endif

#endif
