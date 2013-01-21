
#ifndef __SALSA20_RANDOM_H__
#define __SALSA20_RANDOM_H__ 1

#include <stdint.h>
#include <stdlib.h>

uint32_t  salsa20_random(void);
void      salsa20_random_stir(void);
uint32_t  salsa20_random_uniform(const uint32_t upper_bound);
void      salsa20_random_buf(void * const buf, const size_t size);
int       salsa20_random_close(void);

#endif
