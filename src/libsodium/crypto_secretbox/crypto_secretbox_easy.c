
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "crypto_secretbox.h"
#include "utils.h"

int
crypto_secretbox_easy(unsigned char *c, const unsigned char *m,
                      unsigned long long mlen, const unsigned char *n,
                      const unsigned char *k)
{
    unsigned char *c_boxed;
    unsigned char *m_boxed;
    size_t         c_boxed_len;
    size_t         m_boxed_len;
    int            rc;

    if (mlen > SIZE_MAX - crypto_secretbox_ZEROBYTES) {
        return -1;
    }
    (void) sizeof(char[crypto_secretbox_ZEROBYTES >=
                       crypto_secretbox_BOXZEROBYTES ? 1 : -1]);
    m_boxed_len = (size_t) mlen + crypto_secretbox_ZEROBYTES;
    if ((m_boxed = (unsigned char *) malloc((size_t) m_boxed_len)) == NULL) {
        return -1;
    }
    c_boxed_len = (size_t) mlen + crypto_secretbox_ZEROBYTES;
    if ((c_boxed = (unsigned char *) malloc(c_boxed_len)) == NULL) {
        free(m_boxed);
        return -1;
    }
    memset(m_boxed, 0, crypto_secretbox_ZEROBYTES);
    memcpy(m_boxed + crypto_secretbox_ZEROBYTES, m, mlen);
    rc = crypto_secretbox(c_boxed, m_boxed, m_boxed_len, n, k);
    sodium_memzero(m_boxed, m_boxed_len);
    free(m_boxed);
    if (rc != 0) {
        free(c_boxed);
        return -1;
    }
    assert(m_boxed_len - crypto_secretbox_BOXZEROBYTES ==
           mlen + crypto_secretbox_MACBYTES);
    memcpy(c, c_boxed + crypto_secretbox_BOXZEROBYTES,
           mlen + crypto_secretbox_MACBYTES);
    free(c_boxed);

    return 0;
}

int
crypto_secretbox_open_easy(unsigned char *m, const unsigned char *c,
                           unsigned long long clen, const unsigned char *n,
                           const unsigned char *k)
{
    unsigned char *c_boxed;
    unsigned char *m_boxed;
    size_t         c_boxed_len;
    size_t         m_boxed_len;
    int            rc;

    (void) sizeof(int[crypto_secretbox_BOXZEROBYTES + crypto_secretbox_MACBYTES
                      == crypto_secretbox_ZEROBYTES]);
    if (clen < crypto_secretbox_MACBYTES ||
        clen > SIZE_MAX - crypto_secretbox_BOXZEROBYTES) {
        return -1;
    }
    c_boxed_len = clen + crypto_secretbox_BOXZEROBYTES;
    if ((c_boxed = (unsigned char *) malloc(c_boxed_len)) == NULL) {
        return -1;
    }
    memset(c_boxed, 0, crypto_secretbox_BOXZEROBYTES);
    memcpy(c_boxed + crypto_secretbox_BOXZEROBYTES, c, clen);
    m_boxed_len = c_boxed_len + crypto_secretbox_MACBYTES;
    if ((m_boxed = (unsigned char *) malloc(m_boxed_len)) == NULL) {
        free(c_boxed);
        return -1;
    }
    rc = crypto_secretbox_open(m_boxed, c_boxed,
                               (unsigned long long) c_boxed_len, n, k);
    free(c_boxed);
    if (rc != 0) {
        free(m_boxed);
        return -1;
    }
    memcpy(m, m_boxed + crypto_secretbox_ZEROBYTES,
           clen - crypto_secretbox_MACBYTES);
    free(m_boxed);

    return 0;
}
