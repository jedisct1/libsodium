#ifndef crypto_pwhash_scryptxsalsa208sha256_H
#define crypto_pwhash_scryptxsalsa208sha256_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
# if __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_pwhash_scryptxsalsa208sha256_SALTBYTES 32
SODIUM_EXPORT
size_t crypto_pwhash_scryptxsalsa208sha256_saltbytes(void);

#define crypto_pwhash_scryptxsalsa208sha256_STRBYTES 102
SODIUM_EXPORT
size_t crypto_pwhash_scryptxsalsa208sha256_strbytes(void);

SODIUM_EXPORT
int crypto_pwhash_scryptxsalsa208sha256(unsigned char * const out,
                                        unsigned long long outlen,
                                        const char * const passwd,
                                        unsigned long long passwdlen,
                                        const unsigned char * const salt,
                                        unsigned long long opslimit,
                                        size_t memlimit);

SODIUM_EXPORT
int crypto_pwhash_scryptxsalsa208sha256_str(char out[crypto_pwhash_scryptxsalsa208sha256_STRBYTES],
                                            const char * const passwd,
                                            unsigned long long passwdlen,
                                            unsigned long long opslimit,
                                            size_t memlimit);

SODIUM_EXPORT
int crypto_pwhash_scryptxsalsa208sha256_str_verify(const char str[crypto_pwhash_scryptxsalsa208sha256_STRBYTES],
                                                   const char * const passwd,
                                                   unsigned long long passwdlen);

#ifdef __cplusplus
}
#endif

#endif
