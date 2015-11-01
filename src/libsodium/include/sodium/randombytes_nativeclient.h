
#ifndef randombytes_nativeclient_H
#define randombytes_nativeclient_H

#ifdef __native_client__

#include <stddef.h>
#include <stdint.h>

#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
extern struct randombytes_implementation randombytes_nativeclient_implementation;

SODIUM_EXPORT
const char *randombytes_nativeclient_implementation_name(void);

SODIUM_EXPORT
uint32_t    randombytes_nativeclient(void);

SODIUM_EXPORT
uint32_t    randombytes_nativeclient_uniform(const uint32_t upper_bound);

SODIUM_EXPORT
void        randombytes_nativeclient_buf(void * const buf, const size_t size);

#ifdef __cplusplus
}
#endif

#endif

#endif
