
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"
#include "randombytes.h"
#ifdef _WIN32
# include <Windows.h>
# include <Wincrypt.h>
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

/*
 *  Check to see that binlen is large enough.
 *  Iterate each pair of hexadecimal and convert to binary via sscanf.
 *  sscanf will validate and convert it automatically.
 */
unsigned char *
sodium_hex2bin(unsigned char *bin, size_t binlen,
               const char *hex, const size_t hexlen)
{
    size_t i = (size_t) 0U;

    if (binlen < hexlen / 2) {
        abort();
    }
    while(i < hexlen) {
        if(sscanf(&hex[i*2], "%02x", (unsigned int*) &bin[i]) <= 0) break;
        i++;
    }

    return bin;
}

