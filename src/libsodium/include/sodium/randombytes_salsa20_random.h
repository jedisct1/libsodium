
#ifndef randombytes_salsa20_random_H
#define randombytes_salsa20_random_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *salsa20_random_implementation_name(void);

uint32_t    salsa20_random(void);
void        salsa20_random_stir(void);
uint32_t    salsa20_random_uniform(const uint32_t upper_bound);
void        salsa20_random_buf(void * const buf, const size_t size);
int         salsa20_random_close(void);

#ifdef __cplusplus
}
#endif

#endif
