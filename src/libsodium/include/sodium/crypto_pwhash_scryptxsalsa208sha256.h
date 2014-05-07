#ifndef crypto_pwhash_scryptxsalsa208sha256_H
#define crypto_pwhash_scryptxsalsa208sha256_H

#include <stddef.h>
#include <stdint.h>

#include "export.h"

#define crypto_pwhash_scryptxsalsa208sha256_SALTBYTES 32
#define crypto_pwhash_scryptxsalsa208sha256_STRBYTES 102

#ifdef __cplusplus
# if __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

SODIUM_EXPORT
size_t crypto_pwhash_scryptxsalsa208sha256_saltbytes(void);

SODIUM_EXPORT
size_t crypto_pwhash_scryptxsalsa208sha256_strbytes(void);

SODIUM_EXPORT
int crypto_pwhash_scryptxsalsa208sha256(unsigned char * const out,
                                        unsigned long long outlen,
                                        const char * const passwd,
                                        unsigned long long passwdlen,
                                        const unsigned char * const salt,
                                        size_t memlimit,
                                        unsigned long long opslimit);

SODIUM_EXPORT
int
crypto_pwhash_scryptxsalsa208sha256_str(char * const out,
                                        const char * const passwd,
                                        unsigned long long passwdlen,
                                        size_t memlimit,
                                        unsigned long long opslimit);

#ifdef __cplusplus
}
#endif

#endif
