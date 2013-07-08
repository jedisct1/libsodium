
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "randombytes.h"
#ifdef _WIN32
# include <windows.h>
# include <wincrypt.h>
#endif

void
sodium_memzero(void * const pnt, const size_t len)
{
#ifdef HAVE_SECUREZEROMEMORY
    SecureZeroMemory(pnt, len);
#else
    volatile unsigned char *pnt_ = (volatile unsigned char *) pnt;
    size_t                     i = (size_t) 0U;

    while (i < len) {
        pnt_[i++] = 0U;
    }
#endif
}

int
sodium_memcmp(const void * const b1_, const void * const b2_, size_t size)
{
    const unsigned char *b1 = (const unsigned char *) b1_;
    const unsigned char *b2 = (const unsigned char *) b2_;
    size_t               i;
    unsigned char        d = (unsigned char) 0U;

    for (i = 0U; i < size; i++) {
        d |= b1[i] ^ b2[i];
    }
    return (int) d;
}

unsigned char *
_sodium_alignedcalloc(unsigned char ** const unaligned_p, const size_t len)
{
    unsigned char *aligned;
    unsigned char *unaligned;
    size_t         i;

    if (SIZE_MAX - (size_t) 256U < len ||
        (unaligned = (unsigned char *) malloc(len + (size_t) 256U)) == NULL) {
        *unaligned_p = NULL;
        return NULL;
    }
    *unaligned_p = unaligned;
    for (i = (size_t) 0U; i < len + (size_t) 256U; ++i) {
        unaligned[i] = (unsigned char) rand();
    }
    aligned = unaligned + 64;
    aligned += (ptrdiff_t) 63 & (-(ptrdiff_t) aligned);
    memset(aligned, 0, len);

    return aligned;
}

char *
sodium_bin2hex(char * const hex, const size_t hexlen,
               const unsigned char *bin, const size_t binlen)
{
    static const char hexdigits[16] = "0123456789abcdef";
    size_t            i = (size_t) 0U;
    size_t            j = (size_t) 0U;

    if (binlen >= SIZE_MAX / 2 || hexlen < binlen * 2U) {
        abort();
    }
    while (i < binlen) {
        hex[j++] = hexdigits[bin[i] >> 4];
        hex[j++] = hexdigits[bin[i] & 0xf];
        i++;
    }
    hex[j] = 0;

    return hex;
}
