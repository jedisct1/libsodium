#ifndef crypto_onetimeauth_H
#define crypto_onetimeauth_H

#include "crypto_onetimeauth_poly1305.h"

#define crypto_onetimeauth_implementation_name \
    crypto_onetimeauth_poly1305_ref_implementation_name

#define crypto_onetimeauth crypto_onetimeauth_poly1305_ref
#define crypto_onetimeauth_verify crypto_onetimeauth_poly1305_ref_verify

#endif
