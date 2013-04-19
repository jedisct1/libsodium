
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "randombytes.h"
#ifdef _WIN32
# include <Windows.h>
# include <Wincrypt.h>
#endif

void
sodium_memzero(void * const pnt, const size_t size)
{
#ifdef HAVE_SECUREZEROMEMORY
    SecureZeroMemory(pnt, size);
#else
    volatile unsigned char *pnt_ = (volatile unsigned char *) pnt;
    size_t                     i = (size_t) 0U;

    while (i < size) {
        pnt_[i++] = 0U;
    }
#endif
}

void *
_sodium_alignedcalloc(void ** const unaligned_p, const size_t len)
{
    unsigned char *aligned;
    unsigned char *unaligned;
    size_t         i;

    if (SIZE_MAX - (size_t) 256U < len ||
        (unaligned = malloc(len + (size_t) 256U)) == NULL) {
        *unaligned_p = NULL;
        return NULL;
    }
    *unaligned_p = unaligned;
    for (i = (size_t) 0U; i < len + (size_t) 256U; ++i) {
        unaligned[i] = (unsigned char) random();
    }
    aligned = unaligned + 64;
    aligned += (ptrdiff_t) 63 & (-(ptrdiff_t) aligned);
    memset(aligned, 0, len);

    return aligned;
}
