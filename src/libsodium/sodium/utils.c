
#include "utils.h"
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
