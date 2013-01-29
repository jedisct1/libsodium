#ifndef crypto_sign_ed25519_H
#define crypto_sign_ed25519_H

#define crypto_sign_ed25519_ref_SECRETKEYBYTES 64
#define crypto_sign_ed25519_ref_PUBLICKEYBYTES 32
#define crypto_sign_ed25519_ref_BYTES 64

#ifdef __cplusplus
#include <string>
extern "C" {
#endif
extern int crypto_sign_ed25519_ref(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_ref_open(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_ref_keypair(unsigned char *,unsigned char *);
extern int crypto_sign_ed25519_ref_seed_keypair(unsigned char *,unsigned char *,unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_sign_ed25519 crypto_sign_ed25519_ref
#define crypto_sign_ed25519_open crypto_sign_ed25519_ref_open
#define crypto_sign_ed25519_keypair crypto_sign_ed25519_ref_keypair
#define crypto_sign_ed25519_seed_keypair crypto_sign_ed25519_ref_seed_keypair
#define crypto_sign_ed25519_BYTES crypto_sign_ed25519_ref_BYTES
#define crypto_sign_ed25519_PUBLICKEYBYTES crypto_sign_ed25519_ref_PUBLICKEYBYTES
#define crypto_sign_ed25519_SECRETKEYBYTES crypto_sign_ed25519_ref_SECRETKEYBYTES
#define crypto_sign_ed25519_IMPLEMENTATION "crypto_sign/ed25519/ref"
#ifndef crypto_sign_ed25519_ref_VERSION
#define crypto_sign_ed25519_ref_VERSION "-"
#endif
#define crypto_sign_ed25519_VERSION crypto_sign_ed25519_ref_VERSION

#endif
