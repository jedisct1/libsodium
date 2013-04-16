#ifndef crypto_verify_32_H
#define crypto_verify_32_H

#define crypto_verify_32_BYTES 32
#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_verify_32_ref(const unsigned char *,const unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_verify_32 crypto_verify_32_ref

#endif
