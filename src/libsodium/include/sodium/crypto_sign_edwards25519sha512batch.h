#ifndef crypto_sign_edwards25519sha512batch_H
#define crypto_sign_edwards25519sha512batch_H

#include <stddef.h>
#include "export.h"

#define crypto_sign_edwards25519sha512batch_SECRETKEYBYTES 64U
#define crypto_sign_edwards25519sha512batch_PUBLICKEYBYTES 32U
#define crypto_sign_edwards25519sha512batch_BYTES 64U

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_sign_edwards25519sha512batch_bytes(void);

SODIUM_EXPORT
size_t crypto_sign_edwards25519sha512batch_publickeybytes(void);

SODIUM_EXPORT
size_t crypto_sign_edwards25519sha512batch_secretkeybytes(void);

SODIUM_EXPORT
const char * crypto_sign_edwards25519sha512batch_primitive(void);

SODIUM_EXPORT
int crypto_sign_edwards25519sha512batch(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);

SODIUM_EXPORT
int crypto_sign_edwards25519sha512batch_open(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);

SODIUM_EXPORT
int crypto_sign_edwards25519sha512batch_keypair(unsigned char *,unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_sign_edwards25519sha512batch_ref crypto_sign_edwards25519sha512batch
#define crypto_sign_edwards25519sha512batch_ref_open crypto_sign_edwards25519sha512batch_open
#define crypto_sign_edwards25519sha512batch_ref_keypair crypto_sign_edwards25519sha512batch_keypair

#endif
