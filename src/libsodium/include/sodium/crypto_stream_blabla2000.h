#ifndef crypto_stream_blabla2000_H
#define crypto_stream_blabla2000_H

#include <stddef.h>
#include <stdint.h>
#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_stream_blabla2000_KEYBYTES 32U
SODIUM_EXPORT
size_t crypto_stream_blabla2000_keybytes(void);

#define crypto_stream_blabla2000_NONCEBYTES 16U
SODIUM_EXPORT
size_t crypto_stream_blabla2000_noncebytes(void);

#define crypto_stream_blabla2000_MESSAGEBYTES_MAX SODIUM_SIZE_MAX
SODIUM_EXPORT
size_t crypto_stream_blabla2000_messagebytes_max(void);

SODIUM_EXPORT
int crypto_stream_blabla2000(unsigned char *c, unsigned long long len,
                             const unsigned char *n, const unsigned char *k)
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_stream_blabla2000_xor(unsigned char *c, const unsigned char *m,
                                 unsigned long long mlen, const unsigned char *n,
                                 const unsigned char *k)
            __attribute__ ((nonnull));

SODIUM_EXPORT
void crypto_stream_blabla2000_keygen(unsigned char k[crypto_stream_blabla2000_KEYBYTES])
            __attribute__ ((nonnull));

#ifdef __cplusplus
}
#endif

#endif
