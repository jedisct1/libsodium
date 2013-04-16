#ifndef crypto_scalarmult_curve25519_H
#define crypto_scalarmult_curve25519_H

#define crypto_scalarmult_curve25519_BYTES 32
#define crypto_scalarmult_curve25519_SCALARBYTES 32
#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_scalarmult_curve25519_ref(unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_scalarmult_curve25519_ref_base(unsigned char *,const unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_scalarmult_curve25519 crypto_scalarmult_curve25519_ref
#define crypto_scalarmult_curve25519_base crypto_scalarmult_curve25519_ref_base

#endif
