
#ifndef __SODIUM_UTILS_H__
#define __SODIUM_UTILS_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void sodium_memzero(void * const pnt, const size_t len);

void *_sodium_alignedcalloc(unsigned char ** const unaligned_p,
                            const size_t len);

#ifdef __cplusplus
}
#endif

#endif
