
#include <sys/types.h>

#include <assert.h>
#include <inttypes.h>
#include <limits.h>

#include "crypto_onetimeauth_poly1305.h"
#include "crypto_onetimeauth_poly1305_ref.h"

static const crypto_onetimeauth_poly1305_implementation *implementation =
    &crypto_onetimeauth_poly1305_ref_implementation;

int
crypto_onetimeauth_poly1305_set_implementation(crypto_onetimeauth_poly1305_implementation *impl)
{
    implementation = impl;

    return 0;
}

const char *
crypto_onetimeauth_poly1305_implementation_name(void)
{
    return implementation->implementation_name();
}

int
crypto_onetimeauth_poly1305(unsigned char *out, const unsigned char *in,
                            unsigned long long inlen, const unsigned char *k)
{
    return implementation->onetimeauth(out, in, inlen, k);
}

int
crypto_onetimeauth_poly1305_verify(const unsigned char *h,
                                   const unsigned char *in,
                                   unsigned long long inlen,
                                   const unsigned char *k)
{
    return implementation->onetimeauth_verify(h, in, inlen, k);
}
