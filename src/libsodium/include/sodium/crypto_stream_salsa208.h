#ifndef crypto_stream_salsa208_H
#define crypto_stream_salsa208_H

#define crypto_stream_salsa208_ref_KEYBYTES 32
#define crypto_stream_salsa208_ref_NONCEBYTES 8
#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_stream_salsa208_ref(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_stream_salsa208_ref_xor(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_stream_salsa208_ref_beforenm(unsigned char *,const unsigned char *);
extern int crypto_stream_salsa208_ref_afternm(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_stream_salsa208_ref_xor_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_stream_salsa208 crypto_stream_salsa208_ref
#define crypto_stream_salsa208_xor crypto_stream_salsa208_ref_xor
#define crypto_stream_salsa208_beforenm crypto_stream_salsa208_ref_beforenm
#define crypto_stream_salsa208_afternm crypto_stream_salsa208_ref_afternm
#define crypto_stream_salsa208_xor_afternm crypto_stream_salsa208_ref_xor_afternm
#define crypto_stream_salsa208_KEYBYTES crypto_stream_salsa208_ref_KEYBYTES
#define crypto_stream_salsa208_NONCEBYTES crypto_stream_salsa208_ref_NONCEBYTES
#define crypto_stream_salsa208_BEFORENMBYTES crypto_stream_salsa208_ref_BEFORENMBYTES
#define crypto_stream_salsa208_IMPLEMENTATION "crypto_stream/salsa208/ref"
#ifndef crypto_stream_salsa208_ref_VERSION
#define crypto_stream_salsa208_ref_VERSION "-"
#endif
#define crypto_stream_salsa208_VERSION crypto_stream_salsa208_ref_VERSION

#endif
