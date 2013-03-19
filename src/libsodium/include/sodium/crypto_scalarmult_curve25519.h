#ifndef crypto_scalarmult_curve25519_H
#define crypto_scalarmult_curve25519_H

#define crypto_scalarmult_curve25519_ref_BYTES 32
#define crypto_scalarmult_curve25519_ref_SCALARBYTES 32
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
#define crypto_scalarmult_curve25519_BYTES crypto_scalarmult_curve25519_ref_BYTES
#define crypto_scalarmult_curve25519_SCALARBYTES crypto_scalarmult_curve25519_ref_SCALARBYTES
#define crypto_scalarmult_curve25519_IMPLEMENTATION "crypto_scalarmult/curve25519/ref"
#ifndef crypto_scalarmult_curve25519_ref_VERSION
#define crypto_scalarmult_curve25519_ref_VERSION "-"
#endif
#define crypto_scalarmult_curve25519_VERSION crypto_scalarmult_curve25519_ref_VERSION

#endif
