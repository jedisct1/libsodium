#ifndef crypto_stream_aes128ctr_H
#define crypto_stream_aes128ctr_H

#include "export.h"

#define crypto_stream_aes128ctr_KEYBYTES 16
#define crypto_stream_aes128ctr_NONCEBYTES 16
#define crypto_stream_aes128ctr_BEFORENMBYTES 1408

#ifdef __cplusplus
extern "C" {
#endif


SODIUM_EXPORT
int crypto_stream_aes128ctr(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_stream_aes128ctr_xor(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_stream_aes128ctr_beforenm(unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_stream_aes128ctr_afternm(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

SODIUM_EXPORT
int crypto_stream_aes128ctr_xor_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_stream_aes128ctr_portable crypto_stream_aes128ctr
#define crypto_stream_aes128ctr_portable_xor crypto_stream_aes128ctr_xor
#define crypto_stream_aes128ctr_portable_beforenm crypto_stream_aes128ctr_beforenm
#define crypto_stream_aes128ctr_portable_afternm crypto_stream_aes128ctr_afternm
#define crypto_stream_aes128ctr_portable_xor_afternm crypto_stream_aes128ctr_xor_afternm

#endif
