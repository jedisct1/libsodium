#ifndef crypto_shorthash_H
#define crypto_shorthash_H

#include <stddef.h>

#include "crypto_shorthash_siphash24.h"
#include "export.h"

/**
 * \addgroup hash
 *
 * @todo document
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#define crypto_shorthash_BYTES crypto_shorthash_siphash24_BYTES
  /**
   * @todo document
   */
SODIUM_EXPORT
size_t  crypto_shorthash_bytes(void);

#define crypto_shorthash_KEYBYTES crypto_shorthash_siphash24_KEYBYTES
  /**
   * @todo document
   */
SODIUM_EXPORT
size_t  crypto_shorthash_keybytes(void);

#define crypto_shorthash_PRIMITIVE "siphash24"
  /**
   * @todo document
   */
SODIUM_EXPORT
const char *crypto_shorthash_primitive(void);

SODIUM_EXPORT
  /**
   * @todo document
   */
int crypto_shorthash(unsigned char *out, const unsigned char *in,
                     unsigned long long inlen, const unsigned char *k);

  ///@}
#ifdef __cplusplus
}
#endif

#endif
