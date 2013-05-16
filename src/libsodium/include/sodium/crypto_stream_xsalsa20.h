#ifndef crypto_stream_xsalsa20_H
#define crypto_stream_xsalsa20_H

/*
 *  WARNING: This is just a stream cipher. It is NOT authenticated encryption.
 *  While it provides some protection against eavesdropping, it does NOT
 *  provide any security against active attacks.
 *  Unless you know what you're doing, what you are looking for is probably
 *  the crypto_box functions.
 */

#include <stddef.h>
#include "export.h"

#define crypto_stream_xsalsa20_KEYBYTES 32U
#define crypto_stream_xsalsa20_NONCEBYTES 24U

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_stream_xsalsa20_keybytes(void);

SODIUM_EXPORT
size_t crypto_stream_xsalsa20_noncebytes(void);

SODIUM_EXPORT
const char * crypto_stream_xsalsa20_primitive(void);

SODIUM_EXPORT
int crypto_stream_xsalsa20(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_stream_xsalsa20_xor(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_stream_xsalsa20_ref crypto_stream_xsalsa20
#define crypto_stream_xsalsa20_ref_xor crypto_stream_xsalsa20_xor

#endif
