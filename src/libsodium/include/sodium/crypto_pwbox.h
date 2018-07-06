#ifndef crypto_pwbox_H
#define crypto_pwbox_H

#include <stddef.h>

#include "crypto_pwhash_argon2i.h"
#include "crypto_pwhash_argon2id.h"
#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_pwbox_ALG_ARGON2I13 crypto_pwhash_argon2i_ALG_ARGON2I13
SODIUM_EXPORT
int crypto_pwbox_alg_argon2i13(void);

#define crypto_pwbox_ALG_ARGON2ID13 crypto_pwhash_argon2id_ALG_ARGON2ID13
SODIUM_EXPORT
int crypto_pwbox_alg_argon2id13(void);

#define crypto_pwbox_ALG_DEFAULT crypto_pwhash_ALG_ARGON2ID13
SODIUM_EXPORT
int crypto_pwbox_alg_default(void);

#define crypto_pwbox_PASSWD_MIN crypto_pwhash_argon2id_PASSWD_MIN
SODIUM_EXPORT
size_t crypto_pwbox_passwd_min(void);

#define crypto_pwbox_PASSWD_MAX crypto_pwhash_argon2id_PASSWD_MAX
SODIUM_EXPORT
size_t crypto_pwbox_passwd_max(void);

#define crypto_pwbox_OPSLIMIT_MIN crypto_pwhash_argon2id_OPSLIMIT_MIN
SODIUM_EXPORT
size_t crypto_pwbox_opslimit_min(void);

#define crypto_pwbox_OPSLIMIT_MAX crypto_pwhash_argon2id_OPSLIMIT_MAX
SODIUM_EXPORT
size_t crypto_pwbox_opslimit_max(void);

#define crypto_pwbox_MEMLIMIT_MIN crypto_pwhash_argon2id_MEMLIMIT_MIN
SODIUM_EXPORT
size_t crypto_pwbox_memlimit_min(void);

#define crypto_pwbox_MEMLIMIT_MAX crypto_pwhash_argon2id_MEMLIMIT_MAX
SODIUM_EXPORT
size_t crypto_pwbox_memlimit_max(void);

#define crypto_pwbox_OPSLIMIT_INTERACTIVE crypto_pwhash_argon2id_OPSLIMIT_INTERACTIVE
SODIUM_EXPORT
size_t crypto_pwbox_opslimit_interactive(void);

#define crypto_pwbox_MEMLIMIT_INTERACTIVE crypto_pwhash_argon2id_MEMLIMIT_INTERACTIVE
SODIUM_EXPORT
size_t crypto_pwbox_memlimit_interactive(void);

#define crypto_pwbox_OPSLIMIT_MODERATE crypto_pwhash_argon2id_OPSLIMIT_MODERATE
SODIUM_EXPORT
size_t crypto_pwbox_opslimit_moderate(void);

#define crypto_pwbox_MEMLIMIT_MODERATE crypto_pwhash_argon2id_MEMLIMIT_MODERATE
SODIUM_EXPORT
size_t crypto_pwbox_memlimit_moderate(void);

#define crypto_pwbox_OPSLIMIT_SENSITIVE crypto_pwhash_argon2id_OPSLIMIT_SENSITIVE
SODIUM_EXPORT
size_t crypto_pwbox_opslimit_sensitive(void);

#define crypto_pwbox_MEMLIMIT_SENSITIVE crypto_pwhash_argon2id_MEMLIMIT_SENSITIVE
SODIUM_EXPORT
size_t crypto_pwbox_memlimit_sensitive(void);

#define crypto_pwbox_MACBYTES (4 + 8 + 8 + crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES)
SODIUM_EXPORT
size_t  crypto_secretbox_macbytes(void);

/*
 * The output already includes all the required parameters, including the
 * algorithm identifier. The ciphertext is all that has to be stored in order
 * to unlock using the password.
 */
SODIUM_EXPORT
int crypto_pwbox(unsigned char *c, const unsigned char *m,
                 unsigned long long mlen, const char * const passwd,
                 unsigned long long passwdlen, unsigned long long opslimit,
                 size_t memlimit);

SODIUM_EXPORT
int crypto_pwbox_alg(unsigned char *c, const unsigned char *m,
                     unsigned long long mlen, const char * const passwd,
                     unsigned long long passwdlen, unsigned long long opslimit,
                     size_t memlimit, int alg);

SODIUM_EXPORT
int crypto_pwbox_detached(unsigned char *c, unsigned char *mac,
                          const unsigned char *m, unsigned long long mlen,
                          const char * const passwd,
                          unsigned long long passwdlen,
                          unsigned long long opslimit, size_t memlimit);

SODIUM_EXPORT
int crypto_pwbox_detached_alg(unsigned char *c, unsigned char *mac,
                              const unsigned char *m, unsigned long long mlen,
                              const char * const passwd,
                              unsigned long long passwdlen,
                              unsigned long long opslimit, size_t memlimit,
                              int alg);

/*
 * As the ciphertext includes the parameters for password hashing, it should
 * come from a trusted source.
 */
SODIUM_EXPORT
int crypto_pwbox_open(unsigned char *m, const unsigned char *c,
                      unsigned long long clen, const char * const passwd,
                      unsigned long long passwdlen)
            __attribute__ ((warn_unused_result));

SODIUM_EXPORT
int crypto_pwbox_open_detached(unsigned char *m, const unsigned char *c,
                               const unsigned char *mac,
                               unsigned long long clen,
                               const char * const passwd,
                               unsigned long long passwdlen)
            __attribute__ ((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif
