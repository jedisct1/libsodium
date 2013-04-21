#ifndef crypto_auth_hmacsha512256_H
#define crypto_auth_hmacsha512256_H

#define crypto_auth_hmacsha512256_BYTES 32
#define crypto_auth_hmacsha512256_KEYBYTES 32

#ifdef __cplusplus
extern "C" {
#endif

int crypto_auth_hmacsha512256_ref(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
int crypto_auth_hmacsha512256_ref_verify(const unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);

#ifdef __cplusplus
}
#endif

#define crypto_auth_hmacsha512256 crypto_auth_hmacsha512256_ref
#define crypto_auth_hmacsha512256_verify crypto_auth_hmacsha512256_ref_verify

#endif
