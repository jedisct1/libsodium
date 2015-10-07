#ifndef crypto_aead_aes256gcm_aesni_H
#define crypto_aead_aes256gcm_aesni_H

#include <stddef.h>
#include "export.h"

#ifdef __cplusplus
# if __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_aead_aes256gcm_KEYBYTES  32U
#define crypto_aead_aes256gcm_NSECBYTES 0U
#define crypto_aead_aes256gcm_NPUBBYTES 12U
#define crypto_aead_aes256gcm_ABYTES    16U

SODIUM_EXPORT
int crypto_aead_aes256gcm_aesni_encrypt(unsigned char *c,
                                        unsigned long long *clen_p,
                                        const unsigned char *m,
                                        unsigned long long mlen,
                                        const unsigned char *ad,
                                        unsigned long long adlen,
                                        const unsigned char *nsec,
                                        const unsigned char *npub,
                                        const unsigned char *k);

SODIUM_EXPORT
int crypto_aead_aes256gcm_aesni_decrypt(unsigned char *m,
                                        unsigned long long *mlen_p,
                                        unsigned char *nsec,
                                        const unsigned char *c,
                                        unsigned long long clen,
                                        const unsigned char *ad,
                                        unsigned long long adlen,
                                        const unsigned char *npub,
                                        const unsigned char *k);
#ifdef __cplusplus
}
#endif

#endif
