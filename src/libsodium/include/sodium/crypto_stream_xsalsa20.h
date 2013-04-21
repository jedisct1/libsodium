#ifndef crypto_stream_xsalsa20_H
#define crypto_stream_xsalsa20_H

#include "export.h"

#define crypto_stream_xsalsa20_KEYBYTES 32
#define crypto_stream_xsalsa20_NONCEBYTES 24

#ifdef __cplusplus
extern "C" {
#endif

int crypto_stream_xsalsa20_ref(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
int crypto_stream_xsalsa20_ref_xor(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
int crypto_stream_xsalsa20_ref_beforenm(unsigned char *,const unsigned char *);
int crypto_stream_xsalsa20_ref_afternm(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
int crypto_stream_xsalsa20_ref_xor_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_stream_xsalsa20 crypto_stream_xsalsa20_ref
#define crypto_stream_xsalsa20_xor crypto_stream_xsalsa20_ref_xor
#define crypto_stream_xsalsa20_beforenm crypto_stream_xsalsa20_ref_beforenm
#define crypto_stream_xsalsa20_afternm crypto_stream_xsalsa20_ref_afternm
#define crypto_stream_xsalsa20_xor_afternm crypto_stream_xsalsa20_ref_xor_afternm

#endif
