
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "argon2-core.h"
#include "argon2.h"
#include "crypto_pwhash_argon2id.h"
#include "randombytes.h"
#include "utils.h"

#define STR_HASHBYTES 32U

int
crypto_pwhash_argon2id_alg_argon2id13(void)
{
    return crypto_pwhash_argon2id_ALG_ARGON2ID13;
}

size_t
crypto_pwhash_argon2id_bytes_min(void)
{
    return crypto_pwhash_argon2id_BYTES_MIN;
}

size_t
crypto_pwhash_argon2id_bytes_max(void)
{
    return crypto_pwhash_argon2id_BYTES_MAX;
}

size_t
crypto_pwhash_argon2id_passwd_min(void)
{
    return crypto_pwhash_argon2id_PASSWD_MIN;
}

size_t
crypto_pwhash_argon2id_passwd_max(void)
{
    return crypto_pwhash_argon2id_PASSWD_MAX;
}

size_t
crypto_pwhash_argon2id_saltbytes(void)
{
    return crypto_pwhash_argon2id_SALTBYTES;
}

size_t
crypto_pwhash_argon2id_strbytes(void)
{
    return crypto_pwhash_argon2id_STRBYTES;
}

const char*
crypto_pwhash_argon2id_strprefix(void)
{
    return crypto_pwhash_argon2id_STRPREFIX;
}

size_t
crypto_pwhash_argon2id_opslimit_min(void)
{
    return crypto_pwhash_argon2id_OPSLIMIT_MIN;
}

size_t
crypto_pwhash_argon2id_opslimit_max(void)
{
    return crypto_pwhash_argon2id_OPSLIMIT_MAX;
}

size_t
crypto_pwhash_argon2id_memlimit_min(void)
{
    return crypto_pwhash_argon2id_MEMLIMIT_MIN;
}

size_t
crypto_pwhash_argon2id_memlimit_max(void)
{
    return crypto_pwhash_argon2id_MEMLIMIT_MAX;
}

size_t
crypto_pwhash_argon2id_opslimit_interactive(void)
{
    return crypto_pwhash_argon2id_OPSLIMIT_INTERACTIVE;
}

size_t
crypto_pwhash_argon2id_memlimit_interactive(void)
{
    return crypto_pwhash_argon2id_MEMLIMIT_INTERACTIVE;
}

size_t
crypto_pwhash_argon2id_opslimit_moderate(void)
{
    return crypto_pwhash_argon2id_OPSLIMIT_MODERATE;
}

size_t
crypto_pwhash_argon2id_memlimit_moderate(void)
{
    return crypto_pwhash_argon2id_MEMLIMIT_MODERATE;
}

size_t
crypto_pwhash_argon2id_opslimit_sensitive(void)
{
    return crypto_pwhash_argon2id_OPSLIMIT_SENSITIVE;
}

size_t
crypto_pwhash_argon2id_memlimit_sensitive(void)
{
    return crypto_pwhash_argon2id_MEMLIMIT_SENSITIVE;
}

int
crypto_pwhash_argon2id(unsigned char *const out, unsigned long long outlen,
                       const char *const passwd, unsigned long long passwdlen,
                       const unsigned char *const salt,
                       unsigned long long opslimit, size_t memlimit, int alg)
{
    memset(out, 0, outlen);
    memlimit /= 1024U;
    if (outlen > ARGON2_MAX_OUTLEN || passwdlen > ARGON2_MAX_PWD_LENGTH ||
        opslimit > ARGON2_MAX_TIME || memlimit > ARGON2_MAX_MEMORY) {
        errno = EFBIG;
        return -1;
    }
    if (outlen < ARGON2_MIN_OUTLEN || passwdlen < ARGON2_MIN_PWD_LENGTH ||
        opslimit < ARGON2_MIN_TIME || memlimit < ARGON2_MIN_MEMORY) {
        errno = EINVAL;
        return -1;
    }
    switch (alg) {
    case crypto_pwhash_argon2id_ALG_ARGON2ID13:
        if (argon2id_hash_raw((uint32_t) opslimit, (uint32_t) memlimit,
                              (uint32_t) 1U, passwd, (size_t) passwdlen, salt,
                              (size_t) crypto_pwhash_argon2id_SALTBYTES, out,
                              (size_t) outlen) != ARGON2_OK) {
            return -1; /* LCOV_EXCL_LINE */
        }
        return 0;
    default:
        errno = EINVAL;
        return -1;
    }
}

int
crypto_pwhash_argon2id_str(char out[crypto_pwhash_argon2id_STRBYTES],
                           const char *const passwd,
                           unsigned long long passwdlen,
                           unsigned long long opslimit, size_t memlimit)
{
    unsigned char salt[crypto_pwhash_argon2id_SALTBYTES];

    memset(out, 0, crypto_pwhash_argon2id_STRBYTES);
    memlimit /= 1024U;
    if (passwdlen > ARGON2_MAX_PWD_LENGTH || opslimit > ARGON2_MAX_TIME ||
        memlimit > ARGON2_MAX_MEMORY) {
        errno = EFBIG;
        return -1;
    }
    if (passwdlen < ARGON2_MIN_PWD_LENGTH || opslimit < ARGON2_MIN_TIME ||
        memlimit < ARGON2_MIN_MEMORY) {
        errno = EINVAL;
        return -1;
    }
    randombytes_buf(salt, sizeof salt);
    if (argon2id_hash_encoded((uint32_t) opslimit, (uint32_t) memlimit,
                              (uint32_t) 1U, passwd, (size_t) passwdlen, salt,
                              sizeof salt, STR_HASHBYTES, out,
                              crypto_pwhash_argon2id_STRBYTES) != ARGON2_OK) {
        return -1; /* LCOV_EXCL_LINE */
    }
    return 0;
}

int
crypto_pwhash_argon2id_str_verify(const char str[crypto_pwhash_argon2id_STRBYTES],
                                  const char *const  passwd,
                                  unsigned long long passwdlen)
{
    int verify_ret;

    if (passwdlen > ARGON2_MAX_PWD_LENGTH) {
        errno = EFBIG;
        return -1;
    }
    /* LCOV_EXCL_START */
    if (passwdlen < ARGON2_MIN_PWD_LENGTH) {
        errno = EINVAL;
        return -1;
    }
    /* LCOV_EXCL_STOP */

    verify_ret = argon2id_verify(str, passwd, (size_t) passwdlen);
    if (verify_ret == ARGON2_OK) {
        return 0;
    }
    if (verify_ret == ARGON2_VERIFY_MISMATCH) {
        errno = EINVAL;
    }
    return -1;
}
