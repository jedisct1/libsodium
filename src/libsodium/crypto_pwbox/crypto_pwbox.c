#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "crypto_secretbox.h"
#include "crypto_pwbox.h"
#include "crypto_pwhash.h"
#include "private/common.h"
#include "randombytes.h"
#include "utils.h"

#define PWBOX_MAGIC "spb"

int
crypto_pwbox_alg_argon2i13(void)
{
    return crypto_pwbox_ALG_ARGON2I13;
}

int
crypto_pwbox_alg_argon2id13(void)
{
    return crypto_pwbox_ALG_ARGON2ID13;
}

int
crypto_pwbox_alg_default(void)
{
    return crypto_pwbox_ALG_DEFAULT;
}

size_t
crypto_pwbox_passwd_min(void)
{
    return crypto_pwbox_PASSWD_MIN;
}

size_t
crypto_pwbox_passwd_max(void)
{
    return crypto_pwbox_PASSWD_MAX;
}

size_t
crypto_pwbox_opslimit_min(void)
{
    return crypto_pwbox_OPSLIMIT_MIN;
}

size_t
crypto_pwbox_opslimit_max(void)
{
    return crypto_pwbox_OPSLIMIT_MAX;
}

size_t
crypto_pwbox_memlimit_min(void)
{
    return crypto_pwbox_MEMLIMIT_MIN;
}

size_t
crypto_pwbox_memlimit_max(void)
{
    return crypto_pwbox_MEMLIMIT_MAX;
}

size_t
crypto_pwbox_opslimit_interactive(void)
{
    return crypto_pwbox_OPSLIMIT_INTERACTIVE;
}

size_t
crypto_pwbox_memlimit_interactive(void)
{
    return crypto_pwbox_MEMLIMIT_INTERACTIVE;
}

size_t
crypto_pwbox_opslimit_moderate(void)
{
    return crypto_pwbox_OPSLIMIT_MODERATE;
}

size_t
crypto_pwbox_memlimit_moderate(void)
{
    return crypto_pwbox_MEMLIMIT_MODERATE;
}

size_t
crypto_pwbox_opslimit_sensitive(void)
{
    return crypto_pwbox_OPSLIMIT_SENSITIVE;
}

size_t
crypto_pwbox_memlimit_sensitive(void)
{
    return crypto_pwbox_MEMLIMIT_SENSITIVE;
}

size_t
crypto_pwbox_macbytes(void)
{
    return crypto_pwbox_MACBYTES;
}

int
crypto_pwbox(unsigned char *c, const unsigned char *m, unsigned long long mlen,
             const char * const passwd, unsigned long long passwdlen,
             unsigned long long opslimit, size_t memlimit)
{
    return crypto_pwbox_alg(c, m, mlen, passwd, passwdlen, opslimit, memlimit,
                            crypto_pwbox_ALG_DEFAULT);
}

int
crypto_pwbox_alg(unsigned char *c, const unsigned char *m,
                 unsigned long long mlen, const char * const passwd,
                 unsigned long long passwdlen, unsigned long long opslimit,
                 size_t memlimit, int alg)
{
    return crypto_pwbox_detached_alg(c + crypto_pwbox_MACBYTES, c, m, mlen,
                                     passwd, passwdlen, opslimit, memlimit,
                                     alg);
}

int
crypto_pwbox_detached(unsigned char *c, unsigned char *mac,
                      const unsigned char *m, unsigned long long mlen,
                      const char * const passwd, unsigned long long passwdlen,
                      unsigned long long opslimit, size_t memlimit)
{
    return crypto_pwbox_detached_alg(c, mac, m, mlen, passwd, passwdlen,
                                     opslimit, memlimit,
                                     crypto_pwbox_ALG_DEFAULT);
}

int
crypto_pwbox_detached_alg(unsigned char *c, unsigned char *mac,
                          const unsigned char *m, unsigned long long mlen,
                          const char * const passwd,
                          unsigned long long passwdlen,
                          unsigned long long opslimit, size_t memlimit, int alg)
{
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    unsigned char key[crypto_secretbox_KEYBYTES];
    unsigned char *sbmac;
    int rc;

    COMPILER_ASSERT(crypto_secretbox_NONCEBYTES >= crypto_pwhash_SALTBYTES);

    if (opslimit > UINT64_MAX || memlimit > UINT64_MAX || alg > UINT8_MAX) {
        return -1;
    }

    randombytes_buf(nonce, sizeof nonce);
    if (crypto_pwhash(key, sizeof key, passwd, passwdlen, nonce, opslimit,
                      memlimit, alg) != 0) {
        return -1;
    }

    sbmac = mac + 20 + crypto_secretbox_NONCEBYTES;
    rc = crypto_secretbox_detached(c, sbmac, m, mlen, nonce, key);
    sodium_memzero(key, sizeof key);
    if (rc == 0) {
        memcpy(mac, PWBOX_MAGIC, 3);
        mac[3] = alg & 0xff;
        store64_be(mac + 4, (uint64_t) opslimit);
        store64_be(mac + 12, (uint64_t) memlimit);
        memcpy(mac + 20, nonce, crypto_secretbox_NONCEBYTES);
    }
    return rc;
}

int
crypto_pwbox_open(unsigned char *m, const unsigned char *c,
                  unsigned long long clen, const char * const passwd,
                  unsigned long long passwdlen)
{
    if (clen < crypto_pwbox_MACBYTES) {
        return -1;
    }
    return crypto_pwbox_open_detached(m, c + crypto_pwbox_MACBYTES, c,
                                      clen - crypto_pwbox_MACBYTES, passwd,
                                      passwdlen);
}

int
crypto_pwbox_open_detached(unsigned char *m, const unsigned char *c,
                           const unsigned char *mac, unsigned long long clen,
                           const char * const passwd,
                           unsigned long long passwdlen)
{
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    unsigned char key[crypto_secretbox_KEYBYTES];
    const unsigned char *sbmac;
    unsigned long long opslimit;
    size_t memlimit;
    int alg;
    int rc;

    COMPILER_ASSERT(crypto_secretbox_NONCEBYTES >= crypto_pwhash_SALTBYTES);

    if (memcmp(mac, PWBOX_MAGIC, 3) != 0) {
        return -1;
    }
    alg = (int) mac[3];
    opslimit = (unsigned long long) load64_be(mac + 4);
    memlimit = (size_t) load64_be(mac + 12);
    memcpy(nonce, mac + 20, crypto_secretbox_NONCEBYTES);
    sbmac = mac + 20 + crypto_secretbox_NONCEBYTES;

    if (crypto_pwhash(key, sizeof key, passwd, passwdlen, nonce, opslimit,
                      memlimit, alg) != 0) {
        return -1;
    }

    rc = crypto_secretbox_open_detached(m, c, sbmac, clen, nonce, key);
    sodium_memzero(key, sizeof key);
    return rc;
}
