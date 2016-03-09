#ifndef crypto_otp_H
#define crypto_otp_H

#include "crypto_auth_hmacsha256.h"
#include "crypto_auth_hmacsha512.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif


SODIUM_EXPORT
int crypto_hotp(const char* key,
                size_t keySize,
                uint64_t counter,
                unsigned int digits,
                char* out);

SODIUM_EXPORT
int crypto_hotp_verify(const char* key,
                       size_t keySize,
                       uint64_t counter,
                       unsigned int digits,
                       unsigned int window,
                       const char* m);

SODIUM_EXPORT
int crypto_hotp_sha256(const char* key,
                       size_t keySize,
                       uint64_t counter,
                       unsigned int digits,
                       char* out);

SODIUM_EXPORT
int crypto_hotp_sha256_verify(const char* key,
                              size_t keySize,
                              uint64_t counter,
                              unsigned int digits,
                              unsigned int window,
                              const char* m);

SODIUM_EXPORT
int crypto_hotp_sha512(const char* key,
                       size_t keySize,
                       uint64_t counter,
                       unsigned int digits,
                       char* out);

SODIUM_EXPORT
int crypto_hotp_sha512_verify(const char* key,
                              size_t keySize,
                              uint64_t counter,
                              unsigned int digits,
                              unsigned int window,
                              const char* m);



SODIUM_EXPORT
int crypto_totp(const char* key,
                size_t keySize,
                uint64_t now,
                unsigned int digits,
                char* out);

SODIUM_EXPORT
int crypto_totp_verify(const char* key,
                       size_t keySize,
                       uint64_t now,
                       unsigned int digits,
                       unsigned int window,
                       const char* m);

SODIUM_EXPORT
int crypto_totp_sha256(const char* key,
                       size_t keySize,
                       uint64_t now,
                       uint64_t intialTime,
                       uint64_t step,
                       unsigned int digits,
                       char* out);

SODIUM_EXPORT
int crypto_totp_sha256_verify(const char* key,
                              size_t keySize,
                              uint64_t now,
                              uint64_t initialTime,
                              uint64_t step,
                              unsigned int digits,
                              unsigned int window,
                              const char* m);

SODIUM_EXPORT
int crypto_totp_sha512(const char* key,
                       size_t keySize,
                       uint64_t now,
                       uint64_t intialTime,
                       uint64_t step,
                       unsigned int digits,
                       char* out);

SODIUM_EXPORT
int crypto_totp_sha512_verify(const char* key,
                              size_t keySize,
                              uint64_t now,
                              uint64_t initialTime,
                              uint64_t step,
                              unsigned int digits,
                              unsigned int window,
                              const char* m);


#ifdef __cplusplus
}
#endif

#endif
