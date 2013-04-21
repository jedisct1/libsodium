#ifndef crypto_onetimeauth_poly1305_53_H
#define crypto_onetimeauth_poly1305_53_H

#include "crypto_onetimeauth_poly1305.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
extern struct crypto_onetimeauth_poly1305_implementation
        crypto_onetimeauth_poly1305_53_implementation;

SODIUM_EXPORT
const char *crypto_onetimeauth_poly1305_53_implementation_name(void);

SODIUM_EXPORT
int crypto_onetimeauth_poly1305_53(unsigned char *out,
                                   const unsigned char *in,
                                   unsigned long long inlen,
                                   const unsigned char *k);

SODIUM_EXPORT
int crypto_onetimeauth_poly1305_53_verify(const unsigned char *h,
                                          const unsigned char *in,
                                          unsigned long long inlen,
                                          const unsigned char *k);

#ifdef __cplusplus
}
#endif

#endif
