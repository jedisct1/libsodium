
#include <stdint.h>

#include "../stream_salsa20.h"
#include "crypto_stream_salsa20.h"

extern struct crypto_stream_salsa20_implementation
    crypto_stream_salsa20_xmm6int_implementation;

int crypto_stream_salsa20_xmm6int(unsigned char *c, unsigned long long clen,
                                  const unsigned char *n, const unsigned char *k);

int crypto_stream_salsa20_xmm6int_xor_ic(unsigned char *c, const unsigned char *m,
                                         unsigned long long   mlen,
                                         const unsigned char *n, uint64_t ic,
                                         const unsigned char *k);
