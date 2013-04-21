#ifndef crypto_hash_sha256_H
#define crypto_hash_sha256_H

#define crypto_hash_sha256_BYTES 32

#ifdef __cplusplus
extern "C" {
#endif

int crypto_hash_sha256_ref(unsigned char *,const unsigned char *,unsigned long long);

#ifdef __cplusplus
}
#endif

#define crypto_hash_sha256 crypto_hash_sha256_ref

#endif
