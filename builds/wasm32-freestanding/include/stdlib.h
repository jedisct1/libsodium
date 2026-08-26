#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>
#include <stdint.h>

/* arc4random_buf, malloc and free come from the embedder. */

_Noreturn void abort(void);

uint32_t arc4random(void);
void arc4random_buf(void *buf, size_t size);
void *calloc(size_t nmemb, size_t size);
void free(void *ptr);
void *malloc(size_t size);

#endif
