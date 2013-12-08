
#ifndef randombytes_H
#define randombytes_H

#include <sys/types.h>

#include <stddef.h>
#include <stdint.h>

#include "export.h"

/**
 * \defgroup randbytes Random byte generation
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

  /**
   * @todo
   */
typedef struct randombytes_implementation {
    const char *(*implementation_name)(void);
    uint32_t    (*random)(void);
    void        (*stir)(void);
    uint32_t    (*uniform)(const uint32_t upper_bound);
    void        (*buf)(void * const buf, const size_t size);
    int         (*close)(void);
} randombytes_implementation;

/**
 * @todo
 */
SODIUM_EXPORT
int         randombytes_set_implementation(randombytes_implementation *impl);

/**
 * Fill the specified buffer with size random bytes.
 */
SODIUM_EXPORT
void        randombytes(unsigned char * const buf, const unsigned long long buf_len);

/**
 * Return the implementation name.
 */
SODIUM_EXPORT
const char *randombytes_implementation_name(void);

/**
 * Return a random 32-bit unsigned value.
 */
SODIUM_EXPORT
uint32_t    randombytes_random(void);

/**
 * Generate a new key for the pseudorandom number generator. The file
 * descriptor for the entropy source is kept open, so that the
 * generator can be reseeded even in a chroot() jail.
 *
 */
SODIUM_EXPORT
void        randombytes_stir(void);

/**
 * Return a value between 0 and upper_bound using a uniform
 * distribution.
 */
SODIUM_EXPORT
uint32_t    randombytes_uniform(const uint32_t upper_bound);

/**
 * Fill the specified buffer with size random bytes.
 */
SODIUM_EXPORT
void        randombytes_buf(void * const buf, const size_t size);

/**
 * Close the file descriptor or the handle for the cryptographic
 * service provider.
 */
SODIUM_EXPORT
int         randombytes_close(void);

  /// @}
#ifdef __cplusplus
}
#endif

#endif
