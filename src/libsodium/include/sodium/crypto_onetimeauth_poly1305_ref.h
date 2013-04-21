#ifndef crypto_onetimeauth_poly1305_ref_H
#define crypto_onetimeauth_poly1305_ref_H

#include "crypto_onetimeauth_poly1305.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
extern struct crypto_onetimeauth_poly1305_implementation
        crypto_onetimeauth_poly1305_ref_implementation;

SODIUM_EXPORT
const char *crypto_onetimeauth_poly1305_ref_implementation_name(void);

SODIUM_EXPORT
int crypto_onetimeauth_poly1305_ref(unsigned char *out,
                                    const unsigned char *in,
                                    unsigned long long inlen,
                                    const unsigned char *k);

SODIUM_EXPORT
int crypto_onetimeauth_poly1305_ref_verify(const unsigned char *h,
                                           const unsigned char *in,
                                           unsigned long long inlen,
                                           const unsigned char *k);

#ifdef __cplusplus
}
#endif

#endif
