
#ifndef randombytes_sysrandom_H
#define randombytes_sysrandom_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct randombytes_implementation
       randombytes_sysrandom_implementation(void);

const char *randombytes_sysrandom_implementation_name(void);

uint32_t    randombytes_sysrandom(void);
void        randombytes_sysrandom_stir(void);
uint32_t    randombytes_sysrandom_uniform(const uint32_t upper_bound);
void        randombytes_sysrandom_buf(void * const buf, const size_t size);
int         randombytes_sysrandom_close(void);

#ifdef __cplusplus
}
#endif

#endif
