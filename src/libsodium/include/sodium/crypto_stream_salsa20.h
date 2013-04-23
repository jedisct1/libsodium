#ifndef crypto_stream_salsa20_H
#define crypto_stream_salsa20_H

/*
 *  WARNING: This is just a stream cipher. It is NOT authenticated encryption.
 *  While it provides some protection against eavesdropping, it does NOT
 *  provide any security against active attacks.
 *  Unless you know what you're doing, what you are looking for is probably
 *  the crypto_box functions.
 */

#include "export.h"

#define crypto_stream_salsa20_KEYBYTES 32
#define crypto_stream_salsa20_NONCEBYTES 8

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
int crypto_stream_salsa20(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_stream_salsa20_xor(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_stream_salsa20_beforenm(unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_stream_salsa20_afternm(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_stream_salsa20_xor_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_stream_salsa20_ref crypto_stream_salsa20
#define crypto_stream_salsa20_ref_xor crypto_stream_salsa20_xor
#define crypto_stream_salsa20_ref_beforenm crypto_stream_salsa20_beforenm
#define crypto_stream_salsa20_ref_afternm crypto_stream_salsa20_afternm
#define crypto_stream_salsa20_ref_xor_afternm crypto_stream_salsa20_xor_afternm

#endif
