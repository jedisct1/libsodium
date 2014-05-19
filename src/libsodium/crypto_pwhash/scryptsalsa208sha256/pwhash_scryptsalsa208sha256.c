
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "crypto_pwhash_scryptsalsa208sha256.h"
#include "crypto_scrypt.h"
#include "randombytes.h"
#include "utils.h"

#define SETTING_SIZE(saltbytes) \
    (sizeof "$7$" - 1U) + \
    (1U /* N_log2 */) + (5U /* r */) + (5U /* p */) + BYTES2CHARS(saltbytes)

static int
pickparams(unsigned long long opslimit, const size_t memlimit,
           uint32_t * const N_log2, uint32_t * const p, uint32_t * const r)
{
    unsigned long long maxN;
    unsigned long long maxrp;

    if (opslimit < 32768) {
        opslimit = 32768;
    }
    *r = 8;
    if (opslimit < memlimit / 32) {
        *p = 1;
        maxN = opslimit / (*r * 4);
        for (*N_log2 = 1; *N_log2 < 63; *N_log2 += 1) {
            if ((uint64_t)(1) << *N_log2 > maxN / 2) {
                break;
            }
        }
    } else {
        maxN = memlimit / (*r * 128);
        for (*N_log2 = 1; *N_log2 < 63; *N_log2 += 1) {
            if ((uint64_t) (1) << *N_log2 > maxN / 2) {
                break;
            }
        }
        maxrp = (opslimit / 4) / ((uint64_t) (1) << *N_log2);
        if (maxrp > 0x3fffffff) {
            maxrp = 0x3fffffff;
        }
        *p = (uint32_t) (maxrp) / *r;
    }
    return 0;
}

size_t
crypto_pwhash_scryptsalsa208sha256_saltbytes(void)
{
    return crypto_pwhash_scryptsalsa208sha256_SALTBYTES;
}

size_t
crypto_pwhash_scryptsalsa208sha256_strbytes(void)
{
    return crypto_pwhash_scryptsalsa208sha256_STRBYTES;
}

int
crypto_pwhash_scryptsalsa208sha256(unsigned char * const out,
                                    unsigned long long outlen,
                                    const char * const passwd,
                                    unsigned long long passwdlen,
                                    const unsigned char * const salt,
                                    unsigned long long opslimit,
                                    size_t memlimit)
{
    uint32_t N_log2;
    uint32_t p;
    uint32_t r;

    memset(out, 0, outlen);
    if (passwdlen > SIZE_MAX || outlen > SIZE_MAX) {
        errno = EFBIG;
        return -1;
    }
    if (pickparams(opslimit, memlimit, &N_log2, &p, &r) != 0) {
        errno = EINVAL;
        return -1;
    }
    return crypto_pwhash_scryptsalsa208sha256_ll((const uint8_t *) passwd,
                                                 (size_t) passwdlen,
                                                 (const uint8_t *) salt,
                                                 crypto_pwhash_scryptsalsa208sha256_SALTBYTES,
                                                 (uint64_t) (1) << N_log2, r, p,
                                                 out, (size_t) outlen);
}

int
crypto_pwhash_scryptsalsa208sha256_str(char out[crypto_pwhash_scryptsalsa208sha256_STRBYTES],
                                        const char * const passwd,
                                        unsigned long long passwdlen,
                                        unsigned long long opslimit,
                                        size_t memlimit)
{
    uint8_t         salt[crypto_pwhash_scryptsalsa208sha256_STRSALTBYTES];
    char            setting[crypto_pwhash_scryptsalsa208sha256_STRSETTINGBYTES + 1U];
    escrypt_local_t escrypt_local;
    uint32_t        N_log2;
    uint32_t        p;
    uint32_t        r;

    memset(out, 0, crypto_pwhash_scryptsalsa208sha256_STRBYTES);
    if (passwdlen > SIZE_MAX) {
        errno = EFBIG;
        return -1;
    }
    if (pickparams(opslimit, memlimit, &N_log2, &p, &r) != 0) {
        errno = EINVAL;
        return -1;
    }
    randombytes_buf(salt, sizeof salt);
    if (escrypt_gensalt_r(N_log2, r, p, salt, sizeof salt,
                          (uint8_t *) setting, sizeof setting) == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (escrypt_init_local(&escrypt_local) != 0) {
        return -1;
    }
    if (escrypt_r(&escrypt_local, (const uint8_t *) passwd, (size_t) passwdlen,
                  (const uint8_t *) setting, (uint8_t *) out,
                  crypto_pwhash_scryptsalsa208sha256_STRBYTES) == NULL) {
        escrypt_free_local(&escrypt_local);
        errno = EINVAL;
        return -1;
    }
    escrypt_free_local(&escrypt_local);

    (void) sizeof
        (int[SETTING_SIZE(crypto_pwhash_scryptsalsa208sha256_STRSALTBYTES)
            == crypto_pwhash_scryptsalsa208sha256_STRSETTINGBYTES ? 1 : -1]);
    (void) sizeof
        (int[crypto_pwhash_scryptsalsa208sha256_STRSETTINGBYTES + 1U +
             crypto_pwhash_scryptsalsa208sha256_STRHASHBYTES_ENCODED + 1U
             == crypto_pwhash_scryptsalsa208sha256_STRBYTES ? 1 : -1]);

    return 0;
}

int
crypto_pwhash_scryptsalsa208sha256_str_verify(const char str[crypto_pwhash_scryptsalsa208sha256_STRBYTES],
                                               const char * const passwd,
                                               unsigned long long passwdlen)
{
    char            wanted[crypto_pwhash_scryptsalsa208sha256_STRBYTES];
    escrypt_local_t escrypt_local;
    int             ret = -1;

    if (memchr(str, 0, crypto_pwhash_scryptsalsa208sha256_STRBYTES) !=
        &str[crypto_pwhash_scryptsalsa208sha256_STRBYTES - 1U]) {
        return -1;
    }
    if (escrypt_init_local(&escrypt_local) != 0) {
        return -1;
    }
    if (escrypt_r(&escrypt_local, (const uint8_t *) passwd, (size_t) passwdlen,
                  (const uint8_t *) str, (uint8_t *) wanted,
                  sizeof wanted) == NULL) {
        escrypt_free_local(&escrypt_local);
        return -1;
    }
    escrypt_free_local(&escrypt_local);
    ret = sodium_memcmp(wanted, str, sizeof wanted);
    sodium_memzero(wanted, sizeof wanted);

    return ret;
}
