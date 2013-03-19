#ifndef crypto_hashblocks_sha256_H
#define crypto_hashblocks_sha256_H

#define crypto_hashblocks_sha256_ref_STATEBYTES 32
#define crypto_hashblocks_sha256_ref_BLOCKBYTES 64
#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_hashblocks_sha256_ref(unsigned char *,const unsigned char *,unsigned long long);
#ifdef __cplusplus
}
#endif

#define crypto_hashblocks_sha256 crypto_hashblocks_sha256_ref
#define crypto_hashblocks_sha256_STATEBYTES crypto_hashblocks_sha256_ref_STATEBYTES
#define crypto_hashblocks_sha256_BLOCKBYTES crypto_hashblocks_sha256_ref_BLOCKBYTES
#define crypto_hashblocks_sha256_IMPLEMENTATION "crypto_hashblocks/sha256/ref"
#ifndef crypto_hashblocks_sha256_ref_VERSION
#define crypto_hashblocks_sha256_ref_VERSION "-"
#endif
#define crypto_hashblocks_sha256_VERSION crypto_hashblocks_sha256_ref_VERSION

#endif
