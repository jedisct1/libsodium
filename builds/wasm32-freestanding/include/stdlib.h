#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((noreturn)) void abort(void);

uint32_t arc4random(void);
void    *calloc(size_t nmemb, size_t size);

/* Your application provides these. */
void  arc4random_buf(void *buf, size_t size);
void  free(void *ptr);
void *malloc(size_t size);

#ifdef __cplusplus
}
#endif

#endif
