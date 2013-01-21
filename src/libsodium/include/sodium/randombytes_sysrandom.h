
#ifndef randombytes_sysrandom_H
#define randombytes_sysrandom_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *sysrandom_implementation_name(void);

uint32_t    sysrandom(void);
void        sysrandom_stir(void);
uint32_t    sysrandom_uniform(const uint32_t upper_bound);
void        sysrandom_buf(void * const buf, const size_t size);
int         sysrandom_close(void);

#ifdef __cplusplus
}
#endif

#endif
