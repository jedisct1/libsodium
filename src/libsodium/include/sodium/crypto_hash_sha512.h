#ifndef crypto_hash_sha512_H
#define crypto_hash_sha512_H

#define crypto_hash_sha512_BYTES 64
#define crypto_hash_sha512_BLOCKBYTES 128

#ifdef __cplusplus
extern "C" {
#endif

int crypto_hash_sha512_ref(unsigned char *,const unsigned char *,unsigned long long);

#ifdef __cplusplus
}
#endif

#define crypto_hash_sha512 crypto_hash_sha512_ref

#endif
