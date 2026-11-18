#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

void *memchr(const void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
char *strchr(const char *s, int c);
size_t strlen(const char *s);
int strncmp(const char *s1, const char *s2, size_t n);
char *strrchr(const char *s, int c);

#define memchr(A, B, C) __builtin_memchr((A), (B), (C))
#define memcmp(A, B, C) __builtin_memcmp((A), (B), (C))
#define memcpy(A, B, C) __builtin_memcpy((A), (B), (C))
#define memmove(A, B, C) __builtin_memmove((A), (B), (C))
#define memset(A, B, C) __builtin_memset((A), (B), (C))
#define strchr(A, B) __builtin_strchr((A), (B))
#define strlen(A) __builtin_strlen((A))
#define strncmp(A, B, C) __builtin_strncmp((A), (B), (C))
#define strrchr(A, B) __builtin_strrchr((A), (B))

#endif
