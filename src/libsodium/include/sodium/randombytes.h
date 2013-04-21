
#ifndef randombytes_H
#define randombytes_H

#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>

#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct randombytes_implementation {
    const char *(*implementation_name)(void);
    uint32_t    (*random)(void);
    void        (*stir)(void);
    uint32_t    (*uniform)(const uint32_t upper_bound);
    void        (*buf)(void * const buf, const size_t size);
    int         (*close)(void);
} randombytes_implementation;

int         randombytes_set_implementation(randombytes_implementation *impl);

void        randombytes(unsigned char *buf, unsigned long long size);

const char *randombytes_implementation_name(void);
uint32_t    randombytes_random(void);
void        randombytes_stir(void);
uint32_t    randombytes_uniform(const uint32_t upper_bound);
void        randombytes_buf(void * const buf, const size_t size);
int         randombytes_close(void);

#ifdef __cplusplus
}
#endif

#endif
