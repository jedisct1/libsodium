
#ifndef randombytes_sysrandom_H
#define randombytes_sysrandom_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT const char *sysrandom_implementation_name(void);

SODIUM_EXPORT uint32_t    sysrandom(void);
SODIUM_EXPORT void        sysrandom_stir(void);
SODIUM_EXPORT uint32_t    sysrandom_uniform(const uint32_t upper_bound);
SODIUM_EXPORT void        sysrandom_buf(void * const buf, const size_t size);
SODIUM_EXPORT int         sysrandom_close(void);

#ifdef __cplusplus
}
#endif

#endif
