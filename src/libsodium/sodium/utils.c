
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
 *  First, it verifies the length of the bin buffer to ensure it will
 *  fit all data excluding NULL since it's binary.
 *
 *  Second, it iterate though hex buffer to ensure everything is valid
 *  ascii hexadecimal, accounting for both lowercase and uppercase letters
 *  and pack the data in char pairs, ignoring trailing odd characters.
 * 
 *  Lastly, it then itterates though the data and convert it.
 */
unsigned char *
sodium_hex2bin(unsigned char * const bin, const size_t binlen,
               const char *hex, const size_t hexlen)
{
    unsigned char buf[2];
    size_t i = (size_t) 0U;
    size_t j = (size_t) 0U;
    size_t k = (size_t) 0U;
    
    if(hexlen > SIZE_MAX * 2 || binlen < hexlen / 2) {
        abort();
    }
    for(i = 0; i < hexlen / 2; i++) {
        for(j = 0; j < 2; j++) {
            k = (i * 2) + j;
            if(hex[k] >= '0' && hex[k] <= '9') {
                buf[j] = hex[k] - '0';
            } else if(hex[k] >= 'a' && hex[k] <= 'f') {
                buf[j] = hex[k] - 'a' + 10;
            } else if(hex[k] >= 'A' && hex[k] <= 'F') {
                buf[j] = hex[k] - 'A' + 10;
            } else {
                abort();
            }
        }
        bin[i] = buf[0] << 4;
        bin[i] |= buf[1];
    }
    
    return bin;
}
