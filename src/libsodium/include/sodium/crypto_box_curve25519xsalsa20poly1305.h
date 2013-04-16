#ifndef crypto_box_curve25519xsalsa20poly1305_H
#define crypto_box_curve25519xsalsa20poly1305_H

#define crypto_box_curve25519xsalsa20poly1305_PUBLICKEYBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_SECRETKEYBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_BEFORENMBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_NONCEBYTES 24
#define crypto_box_curve25519xsalsa20poly1305_ZEROBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES 16
#define crypto_box_curve25519xsalsa20poly1305_MACBYTES (crypto_box_curve25519xsalsa20poly1305_ZEROBYTES - crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES)
#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_box_curve25519xsalsa20poly1305_ref(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_ref_open(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_ref_keypair(unsigned char *,unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_ref_beforenm(unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_ref_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_ref_open_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_box_curve25519xsalsa20poly1305 crypto_box_curve25519xsalsa20poly1305_ref
#define crypto_box_curve25519xsalsa20poly1305_open crypto_box_curve25519xsalsa20poly1305_ref_open
#define crypto_box_curve25519xsalsa20poly1305_keypair crypto_box_curve25519xsalsa20poly1305_ref_keypair
#define crypto_box_curve25519xsalsa20poly1305_beforenm crypto_box_curve25519xsalsa20poly1305_ref_beforenm
#define crypto_box_curve25519xsalsa20poly1305_afternm crypto_box_curve25519xsalsa20poly1305_ref_afternm
#define crypto_box_curve25519xsalsa20poly1305_open_afternm crypto_box_curve25519xsalsa20poly1305_ref_open_afternm

#endif
