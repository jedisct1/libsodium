#ifndef crypto_sign_ed25519_H
#define crypto_sign_ed25519_H

#define crypto_sign_ed25519_SECRETKEYBYTES 64
#define crypto_sign_ed25519_PUBLICKEYBYTES 32
#define crypto_sign_ed25519_BYTES 64

#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_sign_ed25519_ref10(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_ref10_open(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_ref10_keypair(unsigned char *,unsigned char *);
extern int crypto_sign_ed25519_ref10_seed_keypair(unsigned char *,unsigned char *,unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_sign_ed25519 crypto_sign_ed25519_ref10
#define crypto_sign_ed25519_open crypto_sign_ed25519_ref10_open
#define crypto_sign_ed25519_keypair crypto_sign_ed25519_ref10_keypair
#define crypto_sign_ed25519_seed_keypair crypto_sign_ed25519_ref10_seed_keypair

#endif
