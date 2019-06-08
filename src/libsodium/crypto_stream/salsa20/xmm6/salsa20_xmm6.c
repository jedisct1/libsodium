
#include <stdint.h>

#include "utils.h"

#include "../stream_salsa20.h"
#include "salsa20_xmm6.h"

#ifdef HAVE_AMD64_ASM

#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_stream_salsa20__xmm6(unsigned char *c, unsigned long long clen,
                                       const unsigned char *n, const unsigned char *k);

extern int crypto_stream_salsa20__xmm6_xor_ic(unsigned char *c, const unsigned char *m,
                                              unsigned long long mlen,
                                              const unsigned char *n,
                                              uint64_t ic, const unsigned char *k);
#ifdef __cplusplus
}
#endif

struct crypto_stream_salsa20_implementation
    crypto_stream_salsa20_xmm6_implementation = {
        SODIUM_C99(.stream =) crypto_stream_salsa20__xmm6,
        SODIUM_C99(.stream_xor_ic =) crypto_stream_salsa20__xmm6_xor_ic,
    };

#endif
