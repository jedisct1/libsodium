
#ifndef stream_chacha20_H
#define stream_chacha20_H

#include <stdint.h>

typedef struct crypto_stream_chacha20_implementation {
    int (*stream)(unsigned char *c, unsigned long long clen,
                  const unsigned char *n, const unsigned char *k);
    int (*stream_ietf_ext)(unsigned char *c, unsigned long long clen,
                           const unsigned char *n, const unsigned char *k);
    int (*stream_xor_ic)(unsigned char *c, const unsigned char *m,
                         unsigned long long mlen,
                         const unsigned char *n, uint64_t ic,
                         const unsigned char *k);
    int (*stream_ietf_ext_xor_ic)(unsigned char *c, const unsigned char *m,
                                  unsigned long long mlen,
                                  const unsigned char *n, uint32_t ic,
                                  const unsigned char *k);
} crypto_stream_chacha20_implementation;

#endif
