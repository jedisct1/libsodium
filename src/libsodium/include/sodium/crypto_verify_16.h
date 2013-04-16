#ifndef crypto_verify_16_H
#define crypto_verify_16_H

#define crypto_verify_16_BYTES 16
#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_verify_16_ref(const unsigned char *,const unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_verify_16 crypto_verify_16_ref

#endif
