
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "crypto_box.h"
#include "utils.h"

int
crypto_box_easy(unsigned char *c, const unsigned char *m,
                unsigned long long mlen, const unsigned char *n,
                const unsigned char *pk, const unsigned char *sk)
{
    unsigned char *c_boxed;
    unsigned char *m_boxed;
    size_t         c_boxed_len;
    size_t         m_boxed_len;
    int            rc;

    if (mlen > SIZE_MAX - crypto_box_ZEROBYTES) {
        return -1;
    }
    (void) sizeof(char[crypto_box_ZEROBYTES >=
                       crypto_box_BOXZEROBYTES ? 1 : -1]);
    m_boxed_len = (size_t) mlen + crypto_box_ZEROBYTES;
    if ((m_boxed = (unsigned char *) malloc((size_t) m_boxed_len)) == NULL) {
        return -1;
    }
    c_boxed_len = (size_t) mlen + crypto_box_ZEROBYTES;
    if ((c_boxed = (unsigned char *) malloc(c_boxed_len)) == NULL) {
        free(m_boxed);
        return -1;
    }
    memset(m_boxed, 0, crypto_box_ZEROBYTES);
    sodium_mlock(m_boxed, m_boxed_len);
    memcpy(m_boxed + crypto_box_ZEROBYTES, m, mlen);
    rc = crypto_box(c_boxed, m_boxed, m_boxed_len, n, pk, sk);
    sodium_munlock(m_boxed, m_boxed_len);
    free(m_boxed);
    if (rc != 0) {
        free(c_boxed);
        return -1;
    }
    assert(m_boxed_len - crypto_box_BOXZEROBYTES ==
           mlen + crypto_box_MACBYTES);
    memcpy(c, c_boxed + crypto_box_BOXZEROBYTES, mlen + crypto_box_MACBYTES);
    free(c_boxed);

    return 0;
}

int
crypto_box_open_easy(unsigned char *m, const unsigned char *c,
                     unsigned long long clen, const unsigned char *n,
                     const unsigned char *pk, const unsigned char *sk)
{
    unsigned char *c_boxed;
    unsigned char *m_boxed;
    size_t         c_boxed_len;
    size_t         m_boxed_len;
    int            rc;

    if (clen < crypto_box_MACBYTES ||
        clen > SIZE_MAX - crypto_box_BOXZEROBYTES) {
        return -1;
    }
    c_boxed_len = clen + crypto_box_BOXZEROBYTES;
    if ((c_boxed = (unsigned char *) malloc(c_boxed_len)) == NULL) {
        return -1;
    }
    memset(c_boxed, 0, crypto_box_BOXZEROBYTES);
    memcpy(c_boxed + crypto_box_BOXZEROBYTES, c, clen);
    m_boxed_len = c_boxed_len + crypto_box_MACBYTES;
    if ((m_boxed = (unsigned char *) malloc(m_boxed_len)) == NULL) {
        free(c_boxed);
        return -1;
    }
    sodium_mlock(m_boxed, m_boxed_len);
    rc = crypto_box_open(m_boxed, c_boxed,
                         (unsigned long long) c_boxed_len, n, pk, sk);
    free(c_boxed);
    if (rc != 0) {
        sodium_munlock(m_boxed, m_boxed_len);
        free(m_boxed);
        return -1;
    }
    memcpy(m, m_boxed + crypto_box_ZEROBYTES, clen - crypto_box_MACBYTES);
    sodium_munlock(m_boxed, m_boxed_len);
    free(m_boxed);

    return 0;
}
