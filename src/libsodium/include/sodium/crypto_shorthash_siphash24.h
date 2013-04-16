#ifndef crypto_shorthash_siphash24_H
#define crypto_shorthash_siphash24_H

#define crypto_shorthash_siphash24_BYTES 8
#define crypto_shorthash_siphash24_KEYBYTES 16
#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_shorthash_siphash24_ref(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_shorthash_siphash24 crypto_shorthash_siphash24_ref

#endif
