#include <errno.h>
#include <stdlib.h>
#include <string.h>

#undef memchr
#undef memcmp
#undef memcpy
#undef memmove
#undef memset
#undef strchr
#undef strcmp
#undef strlen
#undef strncmp
#undef strrchr

/* Without bulk memory, the builtins would just call these functions again. */
#if defined(__wasm_bulk_memory__) || defined(__wasm_bulk_memory_opt__)
#    define HAVE_BULK_MEMORY 1
#endif

__attribute__((weak)) int errno;

__attribute__((weak, no_builtin)) void
abort(void)
{
    __builtin_trap();
}

__attribute__((weak, no_builtin)) uint32_t
arc4random(void)
{
    uint32_t r;

    arc4random_buf(&r, sizeof r);

    return r;
}

__attribute__((weak, no_builtin)) void *
calloc(size_t nmemb, size_t size)
{
    void *ptr;

    if (nmemb > 0 && size > SIZE_MAX / nmemb) {
        return NULL;
    }
    if ((ptr = malloc(nmemb * size)) != NULL) {
        __builtin_memset(ptr, 0, nmemb * size);
    }
    return ptr;
}

__attribute__((weak, no_builtin)) void *
memchr(const void *s, int c, size_t n)
{
    const unsigned char *p = (const unsigned char *) s;

    while (n-- > 0) {
        if (*p == (unsigned char) c) {
            return (void *) (uintptr_t) p;
        }
        p++;
    }
    return NULL;
}

__attribute__((weak, no_builtin)) int
memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = (const unsigned char *) s1;
    const unsigned char *p2 = (const unsigned char *) s2;

    while (n-- > 0) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

__attribute__((weak, no_builtin)) void *
memcpy(void *dest, const void *src, size_t n)
{
#ifdef HAVE_BULK_MEMORY
    return __builtin_memcpy(dest, src, n);
#else
    unsigned char       *d = (unsigned char *) dest;
    const unsigned char *s = (const unsigned char *) src;

    while (n-- > 0) {
        *d++ = *s++;
    }
    return dest;
#endif
}

__attribute__((weak, no_builtin)) void *
memmove(void *dest, const void *src, size_t n)
{
#ifdef HAVE_BULK_MEMORY
    return __builtin_memmove(dest, src, n);
#else
    unsigned char       *d = (unsigned char *) dest;
    const unsigned char *s = (const unsigned char *) src;

    if ((uintptr_t) d <= (uintptr_t) s) {
        while (n-- > 0) {
            *d++ = *s++;
        }
    } else {
        while (n-- > 0) {
            d[n] = s[n];
        }
    }
    return dest;
#endif
}

__attribute__((weak, no_builtin)) void *
memset(void *s, int c, size_t n)
{
#ifdef HAVE_BULK_MEMORY
    return __builtin_memset(s, c, n);
#else
    unsigned char *p = (unsigned char *) s;

    while (n-- > 0) {
        *p++ = (unsigned char) c;
    }
    return s;
#endif
}

__attribute__((weak, no_builtin)) char *
strchr(const char *s, int c)
{
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

__attribute__((weak, no_builtin)) int
strcmp(const char *s1, const char *s2)
{
    while (*s1 != 0 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (unsigned char) *s1 - (unsigned char) *s2;
}

__attribute__((weak, no_builtin)) size_t
strlen(const char *s)
{
    const char *p = s;

    while (*p != 0) {
        p++;
    }
    return (size_t) (p - s);
}

__attribute__((weak, no_builtin)) int
strncmp(const char *s1, const char *s2, size_t n)
{
    while (n-- > 0) {
        if (*s1 != *s2) {
            return (unsigned char) *s1 - (unsigned char) *s2;
        }
        if (*s1 == 0) {
            break;
        }
        s1++;
        s2++;
    }
    return 0;
}

__attribute__((weak, no_builtin)) char *
strrchr(const char *s, int c)
{
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
