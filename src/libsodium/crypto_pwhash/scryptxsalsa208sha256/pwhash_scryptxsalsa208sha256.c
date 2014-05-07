
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#include "crypto_pwhash_scryptxsalsa208sha256.h"
#include "crypto_scrypt.h"

static int
pickparams(const size_t memlimit, unsigned long long opslimit,
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
crypto_pwhash_scryptxsalsa208sha256_bytes(void)
{
    return crypto_pwhash_scryptxsalsa208sha256_BYTES;
}

size_t
crypto_pwhash_scryptxsalsa208sha256_saltbytes(void)
{
    return crypto_pwhash_scryptxsalsa208sha256_SALTBYTES;
}

int
crypto_pwhash_scryptxsalsa208sha256(unsigned char * const out,
                                    const char * const passwd,
                                    unsigned long long passwdlen,
                                    const unsigned char * const salt,
                                    size_t memlimit,
                                    unsigned long long opslimit)
{
    uint32_t N_log2;
    uint32_t p;
    uint32_t r;

    if (passwdlen > SIZE_MAX) {
        errno = EFBIG;
        return -1;
    }
    if (pickparams(memlimit, opslimit, &N_log2, &p, &r) != 0) {
        errno = EINVAL;
        return -1;
    }
    return crypto_scrypt_compat((const uint8_t *) passwd, (size_t) passwdlen,
                                (const uint8_t *) salt,
                                crypto_pwhash_scryptxsalsa208sha256_SALTBYTES,
                                (uint64_t) (1) << N_log2, r, p,
                                out, crypto_pwhash_scryptxsalsa208sha256_BYTES);
}
