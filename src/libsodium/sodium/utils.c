#ifndef __STDC_WANT_LIB_EXT1__
# define __STDC_WANT_LIB_EXT1__ 1
#endif
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_SYS_MMAN_H
# include <sys/mman.h>
#endif

#ifdef _WIN32
# include <windows.h>
# include <wincrypt.h>
#else
# include <unistd.h>
#endif

#include "randombytes.h"
#include "utils.h"

#ifndef ENOSYS
# define ENOSYS ENXIO
#endif

#if defined(_WIN32) && \
    (!defined(WINAPI_FAMILY) || WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
# define WINAPI_DESKTOP
#endif

#define CANARY_SIZE 16U
#define GARBAGE_VALUE 0xdb

#ifndef MAP_NOCORE
# define MAP_NOCORE 0
#endif
#if !defined(MAP_ANON) && defined(MAP_ANONYMOUS)
# define MAP_ANON MAP_ANONYMOUS
#endif
#if defined(WINAPI_DESKTOP) || (defined(MAP_ANON) && defined(HAVE_MMAP)) || \
    defined(HAVE_POSIX_MEMALIGN)
# define HAVE_ALIGNED_MALLOC
#endif
#if defined(HAVE_MPROTECT) && \
    !(defined(PROT_NONE) && defined(PROT_READ) && defined(PROT_WRITE))
# undef HAVE_MPROTECT
#endif
#if defined(HAVE_ALIGNED_MALLOC) && \
    (defined(WINAPI_DESKTOP) || defined(HAVE_MPROTECT))
# define HAVE_PAGE_PROTECTION
#endif
#if !defined(MADV_DODUMP) && defined(MADV_CORE)
# define MADV_DODUMP   MADV_CORE
# define MADV_DONTDUMP MADV_NOCORE
#endif

static size_t        page_size;
static unsigned char canary[CANARY_SIZE];

#ifdef HAVE_WEAK_SYMBOLS
__attribute__((weak)) void
_sodium_memzero_as_a_weak_symbol_to_prevent_lto(void *const  pnt,
                                                const size_t len)
{
    unsigned char *pnt_ = (unsigned char *) pnt;
    size_t         i    = (size_t) 0U;

    while (i < len) {
        pnt_[i++] = 0U;
    }
}
#endif

void
sodium_memzero(void *const pnt, const size_t len)
{
#ifdef _WIN32
    SecureZeroMemory(pnt, len);
#elif defined(HAVE_MEMSET_S)
    if (len > 0U && memset_s(pnt, (rsize_t) len, 0, (rsize_t) len) != 0) {
        abort(); /* LCOV_EXCL_LINE */
    }
#elif defined(HAVE_EXPLICIT_BZERO)
    explicit_bzero(pnt, len);
#elif HAVE_WEAK_SYMBOLS
    _sodium_memzero_as_a_weak_symbol_to_prevent_lto(pnt, len);
#else
    volatile unsigned char *volatile pnt_ =
        (volatile unsigned char *volatile) pnt;
    size_t i = (size_t) 0U;

    while (i < len) {
        pnt_[i++] = 0U;
    }
#endif
}

#ifdef HAVE_WEAK_SYMBOLS
__attribute__((weak)) void
_sodium_dummy_symbol_to_prevent_memcmp_lto(const unsigned char *b1,
                                           const unsigned char *b2,
                                           const size_t         len)
{
    (void) b1;
    (void) b2;
    (void) len;
}
#endif

int
sodium_memcmp(const void *const b1_, const void *const b2_, size_t len)
{
#ifdef HAVE_WEAK_SYMBOLS
    const unsigned char *b1 = (const unsigned char *) b1_;
    const unsigned char *b2 = (const unsigned char *) b2_;
#else
    const volatile unsigned char *volatile b1 =
        (const volatile unsigned char *volatile) b1_;
    const volatile unsigned char *volatile b2 =
        (const volatile unsigned char *volatile) b2_;
#endif
    size_t        i;
    unsigned char d = (unsigned char) 0U;

#if HAVE_WEAK_SYMBOLS
    _sodium_dummy_symbol_to_prevent_memcmp_lto(b1, b2, len);
#endif
    for (i = 0U; i < len; i++) {
        d |= b1[i] ^ b2[i];
    }
    return (1 & ((d - 1) >> 8)) - 1;
}

#ifdef HAVE_WEAK_SYMBOLS
__attribute__((weak)) void
_sodium_dummy_symbol_to_prevent_compare_lto(const unsigned char *b1,
                                            const unsigned char *b2,
                                            const size_t         len)
{
    (void) b1;
    (void) b2;
    (void) len;
}
#endif

int
sodium_compare(const unsigned char *b1_, const unsigned char *b2_, size_t len)
{
#ifdef HAVE_WEAK_SYMBOLS
    const unsigned char *b1 = b1_;
    const unsigned char *b2 = b2_;
#else
    const volatile unsigned char *volatile b1 =
        (const volatile unsigned char *volatile) b1_;
    const volatile unsigned char *volatile b2 =
        (const volatile unsigned char *volatile) b2_;
#endif
    unsigned char gt = 0U;
    unsigned char eq = 1U;
    size_t        i;

#if HAVE_WEAK_SYMBOLS
    _sodium_dummy_symbol_to_prevent_compare_lto(b1, b2, len);
#endif
    i = len;
    while (i != 0U) {
        i--;
        gt |= ((b2[i] - b1[i]) >> 8) & eq;
        eq &= ((b2[i] ^ b1[i]) - 1) >> 8;
    }
    return (int) (gt + gt + eq) - 1;
}

int
sodium_is_zero(const unsigned char *n, const size_t nlen)
{
    size_t        i;
    unsigned char d = 0U;

    for (i = 0U; i < nlen; i++) {
        d |= n[i];
    }
    return 1 & ((d - 1) >> 8);
}

void
sodium_increment(unsigned char *n, const size_t nlen)
{
    size_t        i = 0U;
    uint_fast16_t c = 1U;

#ifdef HAVE_AMD64_ASM
    uint64_t t64, t64_2;
    uint32_t t32;

    if (nlen == 12U) {
        __asm__ __volatile__(
            "xorq %[t64], %[t64] \n"
            "xorl %[t32], %[t32] \n"
            "stc \n"
            "adcq %[t64], (%[out]) \n"
            "adcl %[t32], 8(%[out]) \n"
            : [t64] "=&r"(t64), [t32] "=&r"(t32)
            : [out] "D"(n)
            : "memory", "flags", "cc");
        return;
    } else if (nlen == 24U) {
        __asm__ __volatile__(
            "movq $1, %[t64] \n"
            "xorq %[t64_2], %[t64_2] \n"
            "addq %[t64], (%[out]) \n"
            "adcq %[t64_2], 8(%[out]) \n"
            "adcq %[t64_2], 16(%[out]) \n"
            : [t64] "=&r"(t64), [t64_2] "=&r"(t64_2)
            : [out] "D"(n)
            : "memory", "flags", "cc");
        return;
    } else if (nlen == 8U) {
        __asm__ __volatile__("incq (%[out]) \n"
                             :
                             : [out] "D"(n)
                             : "memory", "flags", "cc");
        return;
    }
#endif
    for (; i < nlen; i++) {
        c += (uint_fast16_t) n[i];
        n[i] = (unsigned char) c;
        c >>= 8;
    }
}

void
sodium_add(unsigned char *a, const unsigned char *b, const size_t len)
{
    size_t        i = 0U;
    uint_fast16_t c = 0U;

#ifdef HAVE_AMD64_ASM
    uint64_t t64, t64_2, t64_3;
    uint32_t t32;

    if (len == 12U) {
        __asm__ __volatile__(
            "movq (%[in]), %[t64] \n"
            "movl 8(%[in]), %[t32] \n"
            "addq %[t64], (%[out]) \n"
            "adcl %[t32], 8(%[out]) \n"
            : [t64] "=&r"(t64), [t32] "=&r"(t32)
            : [in] "S"(b), [out] "D"(a)
            : "memory", "flags", "cc");
        return;
    } else if (len == 24U) {
        __asm__ __volatile__(
            "movq (%[in]), %[t64] \n"
            "movq 8(%[in]), %[t64_2] \n"
            "movq 16(%[in]), %[t64_3] \n"
            "addq %[t64], (%[out]) \n"
            "adcq %[t64_2], 8(%[out]) \n"
            "adcq %[t64_3], 16(%[out]) \n"
            : [t64] "=&r"(t64), [t64_2] "=&r"(t64_2), [t64_3] "=&r"(t64_3)
            : [in] "S"(b), [out] "D"(a)
            : "memory", "flags", "cc");
        return;
    } else if (len == 8U) {
        __asm__ __volatile__(
            "movq (%[in]), %[t64] \n"
            "addq %[t64], (%[out]) \n"
            : [t64] "=&r"(t64)
            : [in] "S"(b), [out] "D"(a)
            : "memory", "flags", "cc");
        return;
    }
#endif
    for (; i < len; i++) {
        c += (uint_fast16_t) a[i] + (uint_fast16_t) b[i];
        a[i] = (unsigned char) c;
        c >>= 8;
    }
}

/* Derived from original code by CodesInChaos */
char *
sodium_bin2hex(char *const hex, const size_t hex_maxlen,
               const unsigned char *const bin, const size_t bin_len)
{
    size_t       i = (size_t) 0U;
    unsigned int x;
    int          b;
    int          c;

    if (bin_len >= SIZE_MAX / 2 || hex_maxlen <= bin_len * 2U) {
        abort(); /* LCOV_EXCL_LINE */
    }
    while (i < bin_len) {
        c = bin[i] & 0xf;
        b = bin[i] >> 4;
        x = (unsigned char) (87U + c + (((c - 10U) >> 8) & ~38U)) << 8 |
            (unsigned char) (87U + b + (((b - 10U) >> 8) & ~38U));
        hex[i * 2U] = (char) x;
        x >>= 8;
        hex[i * 2U + 1U] = (char) x;
        i++;
    }
    hex[i * 2U] = 0U;

    return hex;
}

int
sodium_hex2bin(unsigned char *const bin, const size_t bin_maxlen,
               const char *const hex, const size_t hex_len,
               const char *const ignore, size_t *const bin_len,
               const char **const hex_end)
{
    size_t        bin_pos = (size_t) 0U;
    size_t        hex_pos = (size_t) 0U;
    int           ret     = 0;
    unsigned char c;
    unsigned char c_acc = 0U;
    unsigned char c_alpha0, c_alpha;
    unsigned char c_num0, c_num;
    unsigned char c_val;
    unsigned char state = 0U;

    while (hex_pos < hex_len) {
        c        = (unsigned char) hex[hex_pos];
        c_num    = c ^ 48U;
        c_num0   = (c_num - 10U) >> 8;
        c_alpha  = (c & ~32U) - 55U;
        c_alpha0 = ((c_alpha - 10U) ^ (c_alpha - 16U)) >> 8;
        if ((c_num0 | c_alpha0) == 0U) {
            if (ignore != NULL && state == 0U && strchr(ignore, c) != NULL) {
                hex_pos++;
                continue;
            }
            break;
        }
        c_val = (c_num0 & c_num) | (c_alpha0 & c_alpha);
        if (bin_pos >= bin_maxlen) {
            ret   = -1;
            errno = ERANGE;
            break;
        }
        if (state == 0U) {
            c_acc = c_val * 16U;
        } else {
            bin[bin_pos++] = c_acc | c_val;
        }
        state = ~state;
        hex_pos++;
    }
    if (state != 0U) {
        hex_pos--;
    }
    if (hex_end != NULL) {
        *hex_end = &hex[hex_pos];
    }
    if (bin_len != NULL) {
        *bin_len = bin_pos;
    }
    return ret;
}

/* Derived from original code by Steve "Sc00bz" Thomas. */
static void
base64_encode_chunk(char dest[4], const unsigned char src[3])
{
    int part, diff, i;
    unsigned char chunk[4];

    chunk[0] = src[0] >> 2;
    chunk[1] = (src[0] << 4) | (src[1] >> 4);
    chunk[2] = (src[1] << 2) | (src[2] >> 6);
    chunk[3] = src[2];
    for (i = 0; i < 4; ++i) {
        part = chunk[i] & 63;
        diff = 0x41;

        /* Comments assume ASCII representation, code does not. */
        /* if (part > 25) diff += 'a' - 'A' - 26; */
        diff += ((25 - part) >> 8) & 6;
        /* if (part > 51) diff += '0' - 'A' - 26; */
        diff -= ((51 - part) >> 8) & 75;
        /* if (part > 61) diff += '+' - '0' - 10; */
        diff -= ((61 - part) >> 8) & 15;
        /* if (part > 62) diff += '/' - '+' - 1; */
        diff += ((62 - part) >> 8) & 3;

        dest[i] = part + diff;
    }
}

char *
sodium_bin2base64(char * const base64, const size_t base64_maxlen,
                  const unsigned char * const bin, const size_t bin_len)
{
    size_t        bin_pos = (size_t) 0U;
    size_t        b64_pos = (size_t) 0U;
    int           b64_step;
    unsigned char tmp[3];

    if (bin_len >= SIZE_MAX / 4 * 3  || base64_maxlen <= bin_len * 3U / 4U) {
        abort(); /* LCOV_EXCL_LINE */
    }
    for (bin_pos = (size_t) 0U; bin_pos + 3 <= bin_len; bin_pos += 3) {
        base64_encode_chunk(base64 + b64_pos, bin + bin_pos);
        b64_pos += 4;
    }
    if (bin_pos < bin_len) {
        b64_step = bin_len - bin_pos + 1;

        tmp[0] = bin[bin_pos];
        tmp[1] = (b64_step == 2) ? 0 : bin[bin_pos+1];
        tmp[2] = 0;

        base64_encode_chunk(base64 + b64_pos, tmp);
        b64_pos += b64_step;
    }
    base64[b64_pos] = 0U;
    return base64;
}

static int
base64_decode_chunk(const char src[4])
{
    int res = 0;
    int acc = 0;
    int ch, i, p;

    for (i = 0; i < 4; ++i)
    {
        ch = src[i];
        p  = -1;
        /* if (ch >= 'A' && ch <= 'Z') ret += ch - 'A' + 1; */
        p += (((0x40 - ch) & (ch - 0x5b)) >> 8) & (ch - 64);
        /* if (ch >= 'a' && ch <= 'z') ret += ch - 'a' + 26 + 1; */
        p += (((0x60 - ch) & (ch - 0x7b)) >> 8) & (ch - 70);
        /* if (ch >= '0' && ch <= '9') ret += ch - '0' + 52 + 1; */
        p += (((0x2f - ch) & (ch - 0x3a)) >> 8) & (ch + 5);
        /* if (ch == 0x2b) ret += 62 + 1; */
        p += (((0x2a - ch) & (ch - 0x2c)) >> 8) & 63;
        /* if (ch == 0x2f) ret += 63 + 1; */
        p += (((0x2e - ch) & (ch - 0x30)) >> 8) & 64;

        acc |= p << (18 - 6 * i);
    }

    return acc;
}

int
sodium_base642bin(unsigned char * const bin, const size_t bin_maxlen,
                  const char * const base64, const size_t base64_len,
                  size_t * const bin_len, const char ** const base64_end)
{
    size_t bin_pos = (size_t) 0U;
    size_t b64_pos = (size_t) 0U;
    size_t b64_len = base64_len;
    int    ret     = 0;
    int    val     = 0;
    int    b64_step;
    int    ch;
    char   tmp[4];

    if (((b64_len & 3) == 0) && (b64_len > 0)) {
        b64_step = 0;
        ch = base64[b64_len-1];
        /* if (ch == '=') b64_step += 1; */
        b64_step += (((0x3c - ch) & (ch - 0x3e)) >> 8) & 1;
        ch = base64[b64_len-2];
        b64_step += (((0x3c - ch) & (ch - 0x3e) & -b64_step) >> 8) & 1;
        b64_len -= b64_step;
    }

    while (b64_pos < b64_len) {
        if (b64_pos+1 == b64_len) {
            ret   = -1;
            errno = EINVAL;
            break;
        } else if (bin_pos >= bin_maxlen) {
            ret   = -1;
            errno = ERANGE;
            break;
        } else if (b64_pos+4 <= b64_len) {
            val = base64_decode_chunk(base64 + b64_pos);
            b64_step = 4;
        } else {
            tmp[0] = base64[b64_pos+0];
            tmp[1] = base64[b64_pos+1];
            tmp[2] = (b64_pos+2 < b64_len) ? base64[b64_pos+2] : 0x41;
            tmp[3] = 0x41;
            val = base64_decode_chunk(tmp);
            b64_step = b64_len - b64_pos;
        }
        if (val < 0) {
            break;
        }
        b64_pos += b64_step;
        bin[bin_pos++] = val >> 16;
        if ((bin_pos < bin_maxlen) && (b64_step > 2))
            bin[bin_pos++] = (val >> 8) & 255;
        if ((bin_pos < bin_maxlen) && (b64_step > 3))
            bin[bin_pos++] = val & 255;
    }
    if (base64_end != NULL) {
        *base64_end = &base64[b64_pos];
    }
    if (bin_len != NULL) {
        *bin_len = bin_pos;
    }
    return ret;
}

int
_sodium_alloc_init(void)
{
#ifdef HAVE_ALIGNED_MALLOC
# if defined(_SC_PAGESIZE)
    long page_size_ = sysconf(_SC_PAGESIZE);
    if (page_size_ > 0L) {
        page_size = (size_t) page_size_;
    }
# elif defined(WINAPI_DESKTOP)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    page_size = (size_t) si.dwPageSize;
# endif
    if (page_size < CANARY_SIZE || page_size < sizeof(size_t)) {
        abort(); /* LCOV_EXCL_LINE */
    }
#endif
    randombytes_buf(canary, sizeof canary);

    return 0;
}

int
sodium_mlock(void *const addr, const size_t len)
{
#if defined(MADV_DONTDUMP) && defined(HAVE_MADVISE)
    (void) madvise(addr, len, MADV_DONTDUMP);
#endif
#ifdef HAVE_MLOCK
    return mlock(addr, len);
#elif defined(WINAPI_DESKTOP)
    return -(VirtualLock(addr, len) == 0);
#else
    errno = ENOSYS;
    return -1;
#endif
}

int
sodium_munlock(void *const addr, const size_t len)
{
    sodium_memzero(addr, len);
#if defined(MADV_DODUMP) && defined(HAVE_MADVISE)
    (void) madvise(addr, len, MADV_DODUMP);
#endif
#ifdef HAVE_MLOCK
    return munlock(addr, len);
#elif defined(WINAPI_DESKTOP)
    return -(VirtualUnlock(addr, len) == 0);
#else
    errno = ENOSYS;
    return -1;
#endif
}

static int
_mprotect_noaccess(void *ptr, size_t size)
{
#ifdef HAVE_MPROTECT
    return mprotect(ptr, size, PROT_NONE);
#elif defined(WINAPI_DESKTOP)
    DWORD old;
    return -(VirtualProtect(ptr, size, PAGE_NOACCESS, &old) == 0);
#else
    errno = ENOSYS;
    return -1;
#endif
}

static int
_mprotect_readonly(void *ptr, size_t size)
{
#ifdef HAVE_MPROTECT
    return mprotect(ptr, size, PROT_READ);
#elif defined(WINAPI_DESKTOP)
    DWORD old;
    return -(VirtualProtect(ptr, size, PAGE_READONLY, &old) == 0);
#else
    errno = ENOSYS;
    return -1;
#endif
}

static int
_mprotect_readwrite(void *ptr, size_t size)
{
#ifdef HAVE_MPROTECT
    return mprotect(ptr, size, PROT_READ | PROT_WRITE);
#elif defined(WINAPI_DESKTOP)
    DWORD old;
    return -(VirtualProtect(ptr, size, PAGE_READWRITE, &old) == 0);
#else
    errno = ENOSYS;
    return -1;
#endif
}

#ifdef HAVE_ALIGNED_MALLOC

__attribute__((noreturn)) static void
_out_of_bounds(void)
{
# ifdef SIGSEGV
    raise(SIGSEGV);
# elif defined(SIGKILL)
    raise(SIGKILL);
# endif
    abort();
} /* LCOV_EXCL_LINE */

static inline size_t
_page_round(const size_t size)
{
    const size_t page_mask = page_size - 1U;

    return (size + page_mask) & ~page_mask;
}

static __attribute__((malloc)) unsigned char *
_alloc_aligned(const size_t size)
{
    void *ptr;

# if defined(MAP_ANON) && defined(HAVE_MMAP)
    if ((ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                    MAP_ANON | MAP_PRIVATE | MAP_NOCORE, -1, 0)) ==
        MAP_FAILED) {
        ptr = NULL; /* LCOV_EXCL_LINE */
    }               /* LCOV_EXCL_LINE */
# elif defined(HAVE_POSIX_MEMALIGN)
    if (posix_memalign(&ptr, page_size, size) != 0) {
        ptr = NULL; /* LCOV_EXCL_LINE */
    }               /* LCOV_EXCL_LINE */
# elif defined(WINAPI_DESKTOP)
    ptr = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
# else
#  error Bug
# endif
    return (unsigned char *) ptr;
}

static void
_free_aligned(unsigned char *const ptr, const size_t size)
{
# if defined(MAP_ANON) && defined(HAVE_MMAP)
    (void) munmap(ptr, size);
# elif defined(HAVE_POSIX_MEMALIGN)
    free(ptr);
# elif defined(WINAPI_DESKTOP)
    VirtualFree(ptr, 0U, MEM_RELEASE);
# else
#  error Bug
#endif
}

static unsigned char *
_unprotected_ptr_from_user_ptr(void *const ptr)
{
    uintptr_t      unprotected_ptr_u;
    unsigned char *canary_ptr;
    size_t         page_mask;

    canary_ptr = ((unsigned char *) ptr) - sizeof canary;
    page_mask = page_size - 1U;
    unprotected_ptr_u = ((uintptr_t) canary_ptr & (uintptr_t) ~page_mask);
    if (unprotected_ptr_u <= page_size * 2U) {
        abort(); /* LCOV_EXCL_LINE */
    }
    return (unsigned char *) unprotected_ptr_u;
}

#endif /* HAVE_ALIGNED_MALLOC */

#ifndef HAVE_ALIGNED_MALLOC
static __attribute__((malloc)) void *
_sodium_malloc(const size_t size)
{
    return malloc(size > (size_t) 0U ? size : (size_t) 1U);
}
#else
static __attribute__((malloc)) void *
_sodium_malloc(const size_t size)
{
    void          *user_ptr;
    unsigned char *base_ptr;
    unsigned char *canary_ptr;
    unsigned char *unprotected_ptr;
    size_t         size_with_canary;
    size_t         total_size;
    size_t         unprotected_size;

    if (size >= (size_t) SIZE_MAX - page_size * 4U) {
        errno = ENOMEM;
        return NULL;
    }
    if (page_size <= sizeof canary || page_size < sizeof unprotected_size) {
        abort(); /* LCOV_EXCL_LINE */
    }
    size_with_canary = (sizeof canary) + size;
    unprotected_size = _page_round(size_with_canary);
    total_size       = page_size + page_size + unprotected_size + page_size;
    if ((base_ptr = _alloc_aligned(total_size)) == NULL) {
        return NULL; /* LCOV_EXCL_LINE */
    }
    unprotected_ptr = base_ptr + page_size * 2U;
    _mprotect_noaccess(base_ptr + page_size, page_size);
# ifndef HAVE_PAGE_PROTECTION
    memcpy(unprotected_ptr + unprotected_size, canary, sizeof canary);
# endif
    _mprotect_noaccess(unprotected_ptr + unprotected_size, page_size);
    sodium_mlock(unprotected_ptr, unprotected_size);
    canary_ptr =
        unprotected_ptr + _page_round(size_with_canary) - size_with_canary;
    user_ptr = canary_ptr + sizeof canary;
    memcpy(canary_ptr, canary, sizeof canary);
    memcpy(base_ptr, &unprotected_size, sizeof unprotected_size);
    _mprotect_readonly(base_ptr, page_size);
    assert(_unprotected_ptr_from_user_ptr(user_ptr) == unprotected_ptr);

    return user_ptr;
}
#endif /* !HAVE_ALIGNED_MALLOC */

__attribute__((malloc)) void *
sodium_malloc(const size_t size)
{
    void *ptr;

    if ((ptr = _sodium_malloc(size)) == NULL) {
        return NULL;
    }
    memset(ptr, (int) GARBAGE_VALUE, size);

    return ptr;
}

__attribute__((malloc)) void *
sodium_allocarray(size_t count, size_t size)
{
    size_t total_size;

    if (count > (size_t) 0U && size >= (size_t) SIZE_MAX / count) {
        errno = ENOMEM;
        return NULL;
    }
    total_size = count * size;

    return sodium_malloc(total_size);
}

#ifndef HAVE_ALIGNED_MALLOC
void
sodium_free(void *ptr)
{
    free(ptr);
}
#else
void
sodium_free(void *ptr)
{
    unsigned char *base_ptr;
    unsigned char *canary_ptr;
    unsigned char *unprotected_ptr;
    size_t         total_size;
    size_t         unprotected_size;

    if (ptr == NULL) {
        return;
    }
    canary_ptr      = ((unsigned char *) ptr) - sizeof canary;
    unprotected_ptr = _unprotected_ptr_from_user_ptr(ptr);
    base_ptr        = unprotected_ptr - page_size * 2U;
    memcpy(&unprotected_size, base_ptr, sizeof unprotected_size);
    total_size = page_size + page_size + unprotected_size + page_size;
    _mprotect_readwrite(base_ptr, total_size);
    if (sodium_memcmp(canary_ptr, canary, sizeof canary) != 0) {
        _out_of_bounds();
    }
# ifndef HAVE_PAGE_PROTECTION
    if (sodium_memcmp(unprotected_ptr + unprotected_size, canary,
                      sizeof canary) != 0) {
        _out_of_bounds();
    }
# endif
    sodium_munlock(unprotected_ptr, unprotected_size);
    _free_aligned(base_ptr, total_size);
}
#endif /* HAVE_ALIGNED_MALLOC */

#ifndef HAVE_PAGE_PROTECTION
static int
_sodium_mprotect(void *ptr, int (*cb)(void *ptr, size_t size))
{
    (void) ptr;
    (void) cb;
    errno = ENOSYS;
    return -1;
}
#else
static int
_sodium_mprotect(void *ptr, int (*cb)(void *ptr, size_t size))
{
    unsigned char *base_ptr;
    unsigned char *unprotected_ptr;
    size_t         unprotected_size;

    unprotected_ptr = _unprotected_ptr_from_user_ptr(ptr);
    base_ptr        = unprotected_ptr - page_size * 2U;
    memcpy(&unprotected_size, base_ptr, sizeof unprotected_size);

    return cb(unprotected_ptr, unprotected_size);
}
#endif

int
sodium_mprotect_noaccess(void *ptr)
{
    return _sodium_mprotect(ptr, _mprotect_noaccess);
}

int
sodium_mprotect_readonly(void *ptr)
{
    return _sodium_mprotect(ptr, _mprotect_readonly);
}

int
sodium_mprotect_readwrite(void *ptr)
{
    return _sodium_mprotect(ptr, _mprotect_readwrite);
}
