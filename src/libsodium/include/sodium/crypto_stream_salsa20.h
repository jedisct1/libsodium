#ifndef crypto_stream_salsa20_H
#define crypto_stream_salsa20_H

#define crypto_stream_salsa20_KEYBYTES 32
#define crypto_stream_salsa20_NONCEBYTES 8
#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_stream_salsa20_ref(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_stream_salsa20_ref_xor(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_stream_salsa20_ref_beforenm(unsigned char *,const unsigned char *);
extern int crypto_stream_salsa20_ref_afternm(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_stream_salsa20_ref_xor_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_stream_salsa20 crypto_stream_salsa20_ref
#define crypto_stream_salsa20_xor crypto_stream_salsa20_ref_xor
#define crypto_stream_salsa20_beforenm crypto_stream_salsa20_ref_beforenm
#define crypto_stream_salsa20_afternm crypto_stream_salsa20_ref_afternm
#define crypto_stream_salsa20_xor_afternm crypto_stream_salsa20_ref_xor_afternm

#endif
