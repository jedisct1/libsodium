#ifndef crypto_pwhash_H
#define crypto_pwhash_H

#include <stddef.h>

#include "crypto_pwhash_argon2i.h"
#include "export.h"

#ifdef __cplusplus
# if __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_pwhash_SALTBYTES crypto_pwhash_argon2i_SALTBYTES
SODIUM_EXPORT
size_t crypto_pwhash_saltbytes(void);

#define crypto_pwhash_STRBYTES crypto_pwhash_argon2i_STRBYTES
SODIUM_EXPORT
size_t crypto_pwhash_strbytes(void);

#define crypto_pwhash_STRPREFIX crypto_pwhash_argon2i_STRPREFIX
SODIUM_EXPORT
const char *crypto_pwhash_strprefix(void);

#define crypto_pwhash_OPSLIMIT_INTERACTIVE crypto_pwhash_argon2i_OPSLIMIT_INTERACTIVE
SODIUM_EXPORT
size_t crypto_pwhash_opslimit_interactive(void);

#define crypto_pwhash_MEMLIMIT_INTERACTIVE crypto_pwhash_argon2i_MEMLIMIT_INTERACTIVE
SODIUM_EXPORT
size_t crypto_pwhash_memlimit_interactive(void);

#define crypto_pwhash_OPSLIMIT_MODERATE crypto_pwhash_argon2i_OPSLIMIT_MODERATE
SODIUM_EXPORT
size_t crypto_pwhash_opslimit_moderate(void);

#define crypto_pwhash_MEMLIMIT_MODERATE crypto_pwhash_argon2i_MEMLIMIT_MODERATE
SODIUM_EXPORT
size_t crypto_pwhash_memlimit_moderate(void);

#define crypto_pwhash_OPSLIMIT_SENSITIVE crypto_pwhash_argon2i_OPSLIMIT_SENSITIVE
SODIUM_EXPORT
size_t crypto_pwhash_opslimit_sensitive(void);

#define crypto_pwhash_MEMLIMIT_SENSITIVE crypto_pwhash_argon2i_MEMLIMIT_SENSITIVE
SODIUM_EXPORT
size_t crypto_pwhash_memlimit_sensitive(void);

SODIUM_EXPORT
int crypto_pwhash(unsigned char * const out, unsigned long long outlen,
                  const char * const passwd, unsigned long long passwdlen,
                  const unsigned char * const salt,
                  unsigned long long opslimit, size_t memlimit)
            __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_pwhash_str(char out[crypto_pwhash_STRBYTES],
                      const char * const passwd, unsigned long long passwdlen,
                      unsigned long long opslimit, size_t memlimit)
            __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_pwhash_str_verify(const char str[crypto_pwhash_STRBYTES],
                             const char * const passwd,
                             unsigned long long passwdlen)
            __attribute__ ((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif

