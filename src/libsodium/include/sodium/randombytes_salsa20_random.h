
#ifndef randombytes_salsa20_random_H
#define randombytes_salsa20_random_H

#include <stdint.h>
#include <stdlib.h>

#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

extern struct randombytes_implementation randombytes_salsa20_implementation;

const char *randombytes_salsa20_implementation_name(void);

uint32_t    randombytes_salsa20_random(void);
void        randombytes_salsa20_random_stir(void);
uint32_t    randombytes_salsa20_random_uniform(const uint32_t upper_bound);
void        randombytes_salsa20_random_buf(void * const buf, const size_t size);
int         randombytes_salsa20_random_close(void);

#ifdef __cplusplus
}
#endif

#endif
