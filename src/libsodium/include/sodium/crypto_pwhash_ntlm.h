#ifndef crypto_pwhash_ntlm_H
#define crypto_pwhash_ntlm_H

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_pwhash_ntlm_BYTES 16U
#define crypto_pwhash_ntlm_PASSWD_MIN 0U
#define crypto_pwhash_ntlm_PASSWD_MAX 27U
#define crypto_pwhash_ntlm_SALTBYTES 0U
#define crypto_pwhash_ntlm_STRBYTES 33U
#define crypto_pwhash_ntlm_STRPREFIX ""
#define crypto_pwhash_ntlm_OPSLIMIT_MIN 0U
#define crypto_pwhash_ntlm_OPSLIMIT_MAX ~0U
#define crypto_pwhash_ntlm_MEMLIMIT_MIN 0U
#define crypto_pwhash_ntlm_MEMLIMIT_MAX 0U

SODIUM_EXPORT
int crypto_pwhash_ntlm(unsigned char * const out,
                       unsigned long long outlen,
                       const char * const passwd,
                       unsigned long long passwdlen,
                       const unsigned char * const salt,
                       unsigned long long opslimit, size_t memlimit)
            __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_pwhash_ntlm_str(char out[crypto_pwhash_ntlm_STRBYTES],
                           const char * const passwd,
                           unsigned long long passwdlen,
                           unsigned long long opslimit, size_t memlimit)
            __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_pwhash_ntlm_str_verify(const char str[crypto_pwhash_ntlm_STRBYTES],
                                  const char * const passwd,
                                  unsigned long long passwdlen)
            __attribute__ ((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif
