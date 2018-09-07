#ifndef chacha20_ietf_ext_H
#define chacha20_ietf_ext_H

#include <stdint.h>

/* The ietf_ext variant allows the internal counter to overflow into the IV */
int
crypto_stream_chacha20_ietf_ext_xor_ic(unsigned char *c, const unsigned char *m,
                                       unsigned long long mlen,
                                       const unsigned char *n, uint32_t ic,
                                       const unsigned char *k);
#endif

