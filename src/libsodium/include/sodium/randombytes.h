
#ifndef randombytes_salsa20_random_H
#define randombytes_salsa20_random_H

#include "salsa20_random.h"

#ifdef __cplusplus
extern "C" {
#endif

void      randombytes(unsigned char *, unsigned long long);

#define   randombytes_random  salsa20_random
#define   randombytes_stir    salsa20_random_stir
#define   randombytes_uniform salsa20_random_uniform
#define   randombytes_buf     salsa20_random_buf
#define   randombytes_close   salsa20_random_close

#ifdef __cplusplus
}
#endif

#ifndef randombytes_implementation
#define randombytes_implementation "salsa20_random"
#endif

#endif
