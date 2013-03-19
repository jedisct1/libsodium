#ifndef crypto_stream_salsa2012_H
#define crypto_stream_salsa2012_H

#define crypto_stream_salsa2012_ref_KEYBYTES 32
#define crypto_stream_salsa2012_ref_NONCEBYTES 8
#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_stream_salsa2012_ref(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_stream_salsa2012_ref_xor(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_stream_salsa2012_ref_beforenm(unsigned char *,const unsigned char *);
extern int crypto_stream_salsa2012_ref_afternm(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_stream_salsa2012_ref_xor_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_stream_salsa2012 crypto_stream_salsa2012_ref
#define crypto_stream_salsa2012_xor crypto_stream_salsa2012_ref_xor
#define crypto_stream_salsa2012_beforenm crypto_stream_salsa2012_ref_beforenm
#define crypto_stream_salsa2012_afternm crypto_stream_salsa2012_ref_afternm
#define crypto_stream_salsa2012_xor_afternm crypto_stream_salsa2012_ref_xor_afternm
#define crypto_stream_salsa2012_KEYBYTES crypto_stream_salsa2012_ref_KEYBYTES
#define crypto_stream_salsa2012_NONCEBYTES crypto_stream_salsa2012_ref_NONCEBYTES
#define crypto_stream_salsa2012_BEFORENMBYTES crypto_stream_salsa2012_ref_BEFORENMBYTES
#define crypto_stream_salsa2012_IMPLEMENTATION "crypto_stream/salsa2012/ref"
#ifndef crypto_stream_salsa2012_ref_VERSION
#define crypto_stream_salsa2012_ref_VERSION "-"
#endif
#define crypto_stream_salsa2012_VERSION crypto_stream_salsa2012_ref_VERSION

#endif
