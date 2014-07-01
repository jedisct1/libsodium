#ifndef __STDC_WANT_LIB_EXT1__
# define __STDC_WANT_LIB_EXT1__ 1
#endif
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_SYS_MMAN_H
# include <sys/mman.h>
#endif

#include "utils.h"
#include "randombytes.h"
#ifdef _WIN32
# include <windows.h>
# include <wincrypt.h>
#endif

#ifdef HAVE_WEAK_SYMBOLS
__attribute__((weak)) void
__sodium_dummy_symbol_to_prevent_lto(void * const pnt, const size_t len)
{
    (void) pnt;
    (void) len;
}
#endif

void
sodium_memzero(void * const pnt, const size_t len)
{
#ifdef HAVE_SECUREZEROMEMORY
    SecureZeroMemory(pnt, len);
#elif defined(HAVE_MEMSET_S)
    if (memset_s(pnt, (rsize_t) len, 0, (rsize_t) len) != 0) {
        abort();
    }
#elif defined(HAVE_EXPLICIT_BZERO)
    explicit_bzero(pnt, len);
#elif HAVE_WEAK_SYMBOLS
    memset(pnt, 0, len);
    __sodium_dummy_symbol_to_prevent_lto(pnt, len);
#else
    volatile unsigned char *pnt_ = (volatile unsigned char *) pnt;
    size_t                     i = (size_t) 0U;

    while (i < len) {
        pnt_[i++] = 0U;
    }
#endif
}

int
sodium_memcmp(const void * const b1_, const void * const b2_, size_t len)
{
    const unsigned char *b1 = (const unsigned char *) b1_;
    const unsigned char *b2 = (const unsigned char *) b2_;
    size_t               i;
    unsigned char        d = (unsigned char) 0U;

    for (i = 0U; i < len; i++) {
        d |= b1[i] ^ b2[i];
    }
    return (int) ((1 & ((d - 1) >> 8)) - 1);
}

unsigned char *
_sodium_alignedcalloc(unsigned char ** const unaligned_p, const size_t len)
{
    unsigned char *aligned;
    unsigned char *unaligned;
    size_t         i;

    if (SIZE_MAX - (size_t) 256U < len ||
        (unaligned = (unsigned char *) malloc(len + (size_t) 256U)) == NULL) {
        *unaligned_p = NULL;
        return NULL;
    }
    *unaligned_p = unaligned;
#ifdef HAVE_ARC4RANDOM_BUF
    (void) i;
    arc4random_buf(unaligned, len + (size_t) 256U);
#else
    for (i = (size_t) 0U; i < len + (size_t) 256U; ++i) {
        unaligned[i] = (unsigned char) rand();
    }
#endif
    aligned = unaligned + 64;
    aligned += (ptrdiff_t) 63 & (-(ptrdiff_t) aligned);
    memset(aligned, 0, len);

    return aligned;
}

char *
sodium_bin2hex(char * const hex, const size_t hex_maxlen,
               const unsigned char * const bin, const size_t bin_len)
{
    static const char hexdigits[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };
    size_t            i = (size_t) 0U;
    size_t            j = (size_t) 0U;

    if (bin_len >= SIZE_MAX / 2 || hex_maxlen < bin_len * 2U) {
        abort();
    }
    while (i < bin_len) {
        hex[j++] = hexdigits[bin[i] >> 4];
        hex[j++] = hexdigits[bin[i] & 0xf];
        i++;
    }
    hex[j] = 0;

    return hex;
}

int
sodium_hex2bin(unsigned char * const bin, const size_t bin_maxlen,
               const char * const hex, const size_t hex_len,
               const char * const ignore, size_t * const bin_len,
               const char ** const hex_end)
{
    size_t        bin_pos = (size_t) 0U;
    size_t        hex_pos = (size_t) 0U;
    int           ret = 0;
    unsigned char c;
    unsigned char c_acc = 0U;
    unsigned char c_num;
    unsigned char c_val;
    unsigned char state = 0U;

    while (hex_pos < hex_len) {
        c = (unsigned char) hex[hex_pos];
        if ((c_num = c ^ 48U) < 10U) {
            c_val = c_num;
        } else if ((c_num = (c & ~32U)) > 64 && c_num < 71U) {
            c_val = c_num - 55U;
        } else if (ignore != NULL && strchr(ignore, c) != NULL && state == 0U) {
            hex_pos++;
            continue;
        } else {
            break;
        }
        if (bin_pos >= bin_maxlen) {
            ret = -1;
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

int
sodium_mlock(void * const addr, const size_t len)
{
#ifdef MADV_DONTDUMP
    (void) madvise(addr, len, MADV_DONTDUMP);
#endif
#ifdef HAVE_MLOCK
    return mlock(addr, len);
#elif defined(HAVE_VIRTUALLOCK)
    return -(VirtualLock(addr, len) == 0);
#else
    errno = ENOSYS;
    return -1;
#endif
}

int
sodium_munlock(void * const addr, const size_t len)
{
    sodium_memzero(addr, len);
#ifdef MADV_DODUMP
    (void) madvise(addr, len, MADV_DODUMP);
#endif
#ifdef HAVE_MLOCK
    return munlock(addr, len);
#elif defined(HAVE_VIRTUALLOCK)
    return -(VirtualUnlock(addr, len) == 0);
#else
    errno = ENOSYS;
    return -1;
#endif
}
