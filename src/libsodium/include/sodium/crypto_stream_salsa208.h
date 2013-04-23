#ifndef crypto_stream_salsa208_H
#define crypto_stream_salsa208_H

#include "export.h"

#define crypto_stream_salsa208_KEYBYTES 32
#define crypto_stream_salsa208_NONCEBYTES 8

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
int crypto_stream_salsa208(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_stream_salsa208_xor(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_stream_salsa208_beforenm(unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_stream_salsa208_afternm(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_stream_salsa208_xor_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_stream_salsa208_ref crypto_stream_salsa208
#define crypto_stream_salsa208_ref_xor crypto_stream_salsa208_xor
#define crypto_stream_salsa208_ref_beforenm crypto_stream_salsa208_beforenm
#define crypto_stream_salsa208_ref_afternm crypto_stream_salsa208_afternm
#define crypto_stream_salsa208_ref_xor_afternm crypto_stream_salsa208_xor_afternm

#endif
