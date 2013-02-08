#ifndef crypto_shorthash_siphash24_H
#define crypto_shorthash_siphash24_H

#define crypto_shorthash_siphash24_ref_BYTES 8
#ifdef __cplusplus
#include <string>
extern "C" {
#endif
extern int crypto_shorthash_siphash24_ref(unsigned char *,const unsigned char *,unsigned long long);
#ifdef __cplusplus
}
#endif

#define crypto_shorthash_siphash24 crypto_shorthash_siphash24_ref
#define crypto_shorthash_siphash24_BYTES crypto_shorthash_siphash24_ref_BYTES
#define crypto_shorthash_siphash24_IMPLEMENTATION
#ifndef crypto_shorthash_siphash24_ref_VERSION
#define crypto_shorthash_siphash24_ref_VERSION "-"
#endif
#define crypto_shorthash_siphash24_VERSION crypto_shorthash_siphash24_ref_VERSION

#endif
