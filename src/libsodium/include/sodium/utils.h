
#ifndef __SODIUM_UTILS_H__
#define __SODIUM_UTILS_H__

#include <stddef.h>

#include "export.h"

/** \defgroup util Util Functions
 *
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__cplusplus) || !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
# define _SODIUM_C99(X)
#else
# define _SODIUM_C99(X) X
#endif

  /** @todo
   *
   */
unsigned char *_sodium_alignedcalloc(unsigned char ** const unaligned_p,
                                     const size_t len);

/**
 * Securely wipe a region in memory.
 *
 * @param[in] pnt  the region on memory
 * @param[in] size the size of the region to be wiped.
 *
 * @warning If a region has been allocated on the heap, you still have
 * to make sure that it can't get swapped to disk, possibly using
 * mlock(2).
 *
 */
SODIUM_EXPORT
void sodium_memzero(void * const pnt, const size_t len);

  /** @todo
   *
   */
SODIUM_EXPORT
int sodium_memcmp(const void * const b1_, const void * const b2_, size_t size);

  /** @todo
   *
   */
SODIUM_EXPORT
char *sodium_bin2hex(char * const hex, const size_t hexlen,
                     const unsigned char *bin, const size_t binlen);

  /// @}
#ifdef __cplusplus
}
#endif

#endif
