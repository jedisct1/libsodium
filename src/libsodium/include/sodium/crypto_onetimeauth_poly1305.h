#ifndef crypto_onetimeauth_poly1305_H
#define crypto_onetimeauth_poly1305_H

#define crypto_onetimeauth_poly1305_BYTES 16
#define crypto_onetimeauth_poly1305_KEYBYTES 32
#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_onetimeauth_poly1305_ref(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_onetimeauth_poly1305_ref_verify(const unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);

extern int crypto_onetimeauth_poly1305_53(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_onetimeauth_poly1305_53_verify(const unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_onetimeauth_poly1305 crypto_onetimeauth_poly1305_ref
#define crypto_onetimeauth_poly1305_verify crypto_onetimeauth_poly1305_ref_verify

#endif
