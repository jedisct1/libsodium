
#ifndef randombytes_H
#define randombytes_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>

typedef struct randombytes_implementation {
    const char *(*randombytes_implementation_name)(void);
    uint32_t    (*randombytes_random)(void);
    void        (*randombytes_stir)(void);
    uint32_t    (*randombytes_uniform)(const uint32_t upper_bound);
    void        (*randombytes_buf)(void * const buf, const size_t size);
    int         (*randombytes_close)(void);
} randombytes_implementation;

SODIUM_EXPORT int         randombytes_set_implementation(randombytes_implementation *impl);

SODIUM_EXPORT void        randombytes(unsigned char *buf, unsigned long long size);

SODIUM_EXPORT const char *randombytes_implementation_name(void);
SODIUM_EXPORT uint32_t    randombytes_random(void);
SODIUM_EXPORT void        randombytes_stir(void);
SODIUM_EXPORT uint32_t    randombytes_uniform(const uint32_t upper_bound);
SODIUM_EXPORT void        randombytes_buf(void * const buf, const size_t size);
SODIUM_EXPORT int         randombytes_close(void);

#ifdef __cplusplus
}
#endif

#endif
