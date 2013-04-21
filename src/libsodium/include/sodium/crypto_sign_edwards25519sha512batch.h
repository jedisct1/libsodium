#ifndef crypto_sign_edwards25519sha512batch_H
#define crypto_sign_edwards25519sha512batch_H

#define crypto_sign_edwards25519sha512batch_SECRETKEYBYTES 64
#define crypto_sign_edwards25519sha512batch_PUBLICKEYBYTES 32
#define crypto_sign_edwards25519sha512batch_BYTES 64

#ifdef __cplusplus
extern "C" {
#endif

int crypto_sign_edwards25519sha512batch_ref(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
int crypto_sign_edwards25519sha512batch_ref_open(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
int crypto_sign_edwards25519sha512batch_ref_keypair(unsigned char *,unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_sign_edwards25519sha512batch crypto_sign_edwards25519sha512batch_ref
#define crypto_sign_edwards25519sha512batch_open crypto_sign_edwards25519sha512batch_ref_open
#define crypto_sign_edwards25519sha512batch_keypair crypto_sign_edwards25519sha512batch_ref_keypair

#endif
