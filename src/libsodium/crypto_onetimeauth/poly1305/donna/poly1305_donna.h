#ifndef __POLY1305_DONNA_H__
#define __POLY1305_DONNA_H__

#include <stddef.h>

#include "crypto_onetimeauth_poly1305.h"

typedef crypto_onetimeauth_poly1305_state poly1305_context;

extern struct crypto_onetimeauth_poly1305_implementation
    crypto_onetimeauth_poly1305_donna_implementation;

const char *crypto_onetimeauth_poly1305_donna_implementation_name(void);

int crypto_onetimeauth_poly1305_donna(unsigned char *out,
                                      const unsigned char *in,
                                      unsigned long long inlen,
                                      const unsigned char *k);

int crypto_onetimeauth_poly1305_donna_verify(const unsigned char *h,
                                             const unsigned char *in,
                                             unsigned long long inlen,
                                             const unsigned char *k);
#endif /* POLY1305_DONNA_H */
