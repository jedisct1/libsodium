#include <stddef.h>
#include <stdint.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

int errno;

void *calloc(size_t nmemb, size_t size) {
    void *ptr;

    if (nmemb > (size_t)0U && size > SIZE_MAX / nmemb) {
        return NULL;
    }
    if ((ptr = malloc(nmemb * size)) != NULL) {
        memset(ptr, 0, nmemb * size);
    }
    return ptr;
}

_Noreturn void abort(void) {
    __builtin_trap();
}

uint32_t arc4random(void) {
    uint32_t r;

    arc4random_buf(&r, sizeof r);

    return r;
}

void *memchr(const void *s, int c, size_t n) {
    const unsigned char *p = (const unsigned char *)s;

    while (n-- > (size_t)0U) {
        if (*p == (unsigned char) c) {
            return (void *) (uintptr_t) p;
        }
        p++;
    }
    return NULL;
}

size_t strlen(const char *s) {
    const char *p = s;

    while (*p != 0) {
        p++;
    }
    return (size_t)(p - s);
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n-- > (size_t) 0U) {
        if (*s1 != *s2) {
            return (int) (unsigned char) *s1 - (int) (unsigned char) *s2;
        }
        if (*s1 == 0) {
            break;
        }
        s1++;
        s2++;
    }
    return 0;
}

char *strchr(const char *s, int c) {
    for (;;) {
        if (*s == (char) c) {
            return (char *) (uintptr_t) s;
        }
        if (*s == 0) {
            return NULL;
        }
        s++;
    }
}

char *strrchr(const char *s, int c) {
    const char *found = NULL;

    for (;;) {
        if (*s == (char) c) {
            found = s;
        }
        if (*s == 0) {
            break;
        }
        s++;
    }
    return (char *) (uintptr_t) found;
}
