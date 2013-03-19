#ifndef crypto_onetimeauth_poly1305_H
#define crypto_onetimeauth_poly1305_H

#define crypto_onetimeauth_poly1305_ref_BYTES 16
#define crypto_onetimeauth_poly1305_ref_KEYBYTES 32
#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_onetimeauth_poly1305_ref(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_onetimeauth_poly1305_ref_verify(const unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_onetimeauth_poly1305 crypto_onetimeauth_poly1305_ref
#define crypto_onetimeauth_poly1305_verify crypto_onetimeauth_poly1305_ref_verify
#define crypto_onetimeauth_poly1305_BYTES crypto_onetimeauth_poly1305_ref_BYTES
#define crypto_onetimeauth_poly1305_KEYBYTES crypto_onetimeauth_poly1305_ref_KEYBYTES
#define crypto_onetimeauth_poly1305_IMPLEMENTATION "crypto_onetimeauth/poly1305/ref"
#ifndef crypto_onetimeauth_poly1305_ref_VERSION
#define crypto_onetimeauth_poly1305_ref_VERSION "-"
#endif
#define crypto_onetimeauth_poly1305_VERSION crypto_onetimeauth_poly1305_ref_VERSION

#endif
