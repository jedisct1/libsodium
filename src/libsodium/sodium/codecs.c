#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "private/common.h"
#include "utils.h"

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
        sodium_misuse(); /* LCOV_EXCL_LINE */
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
        errno = EINVAL;
        ret = -1;
    }
    if (ret != 0) {
        bin_pos = (size_t) 0U;
    }
    if (hex_end != NULL) {
        *hex_end = &hex[hex_pos];
    } else if (hex_pos != hex_len) {
        errno = EINVAL;
        ret = -1;
    }
    if (bin_len != NULL) {
        *bin_len = bin_pos;
    }
    return ret;
}

/*
 * Some macros for constant-time comparisons. These work over values in
 * the 0..255 range. Returned value is 0x00 on "false", 0xFF on "true".
 *
 * Original code by Thomas Pornin.
 */
#define EQ(x, y) \
    ((((0U - ((unsigned int) (x) ^ (unsigned int) (y))) >> 8) & 0xFF) ^ 0xFF)
#define GT(x, y) ((((unsigned int) (y) - (unsigned int) (x)) >> 8) & 0xFF)
#define GE(x, y) (GT(y, x) ^ 0xFF)
#define LT(x, y) GT(y, x)
#define LE(x, y) GE(y, x)

static int
b64_byte_to_char(unsigned int x)
{
    return (LT(x, 26) & (x + 'A')) |
           (GE(x, 26) & LT(x, 52) & (x + ('a' - 26))) |
           (GE(x, 52) & LT(x, 62) & (x + ('0' - 52))) | (EQ(x, 62) & '+') |
           (EQ(x, 63) & '/');
}

static unsigned int
b64_char_to_byte(int c)
{
    const unsigned int x =
        (GE(c, 'A') & LE(c, 'Z') & (c - 'A')) |
        (GE(c, 'a') & LE(c, 'z') & (c - ('a' - 26))) |
        (GE(c, '0') & LE(c, '9') & (c - ('0' - 52))) | (EQ(c, '+') & 62) |
        (EQ(c, '/') & 63);

    return x | (EQ(x, 0) & (EQ(c, 'A') ^ 0xFF));
}

static int
b64_byte_to_urlsafe_char(unsigned int x)
{
    return (LT(x, 26) & (x + 'A')) |
           (GE(x, 26) & LT(x, 52) & (x + ('a' - 26))) |
           (GE(x, 52) & LT(x, 62) & (x + ('0' - 52))) | (EQ(x, 62) & '-') |
           (EQ(x, 63) & '_');
}

static unsigned int
b64_urlsafe_char_to_byte(int c)
{
    const unsigned x =
        (GE(c, 'A') & LE(c, 'Z') & (c - 'A')) |
        (GE(c, 'a') & LE(c, 'z') & (c - ('a' - 26))) |
        (GE(c, '0') & LE(c, '9') & (c - ('0' - 52))) | (EQ(c, '-') & 62) |
        (EQ(c, '_') & 63);

    return x | (EQ(x, 0) & (EQ(c, 'A') ^ 0xFF));
}


#define VARIANT_NO_PADDING_MASK 0x2U
#define VARIANT_URLSAFE_MASK    0x4U

static void
sodium_base64_check_variant(const int variant)
{
    if ((((unsigned int) variant) & ~ 0x6U) != 0x1U) {
        sodium_misuse();
    }
}

size_t
sodium_base64_encoded_len(const size_t bin_len, const int variant)
{
    sodium_base64_check_variant(variant);

    return sodium_base64_ENCODED_LEN(bin_len, variant);
}

char *
sodium_bin2base64(char * const b64, const size_t b64_maxlen,
                  const unsigned char * const bin, const size_t bin_len,
                  const int variant)
{
    size_t       acc_len = (size_t) 0;
    size_t       b64_len;
    size_t       b64_pos = (size_t) 0;
    size_t       bin_pos = (size_t) 0;
    size_t       nibbles;
    size_t       remainder;
    unsigned int acc = 0U;

    sodium_base64_check_variant(variant);
    nibbles = bin_len / 3;
    remainder = bin_len - 3 * nibbles;
    b64_len = nibbles * 4;
    if (remainder != 0) {
        if ((((unsigned int) variant) & VARIANT_NO_PADDING_MASK) == 0U) {
            b64_len += 4;
        } else {
            b64_len += 2 + (remainder >> 1);
        }
    }
    if (b64_maxlen <= b64_len) {
        sodium_misuse();
    }
    if ((((unsigned int) variant) & VARIANT_URLSAFE_MASK) != 0U) {
        while (bin_pos < bin_len) {
            acc = (acc << 8) + bin[bin_pos++];
            acc_len += 8;
            while (acc_len >= 6) {
                acc_len -= 6;
                b64[b64_pos++] = (char) b64_byte_to_urlsafe_char((acc >> acc_len) & 0x3F);
            }
        }
        if (acc_len > 0) {
            b64[b64_pos++] = (char) b64_byte_to_urlsafe_char((acc << (6 - acc_len)) & 0x3F);
        }
    } else {
        while (bin_pos < bin_len) {
            acc = (acc << 8) + bin[bin_pos++];
            acc_len += 8;
            while (acc_len >= 6) {
                acc_len -= 6;
                b64[b64_pos++] = (char) b64_byte_to_char((acc >> acc_len) & 0x3F);
            }
        }
        if (acc_len > 0) {
            b64[b64_pos++] = (char) b64_byte_to_char((acc << (6 - acc_len)) & 0x3F);
        }
    }
    assert(b64_pos <= b64_len);
    while (b64_pos < b64_len) {
        b64[b64_pos++] = '=';
    }
    do {
        b64[b64_pos++] = 0U;
    } while (b64_pos < b64_maxlen);

    return b64;
}

static int
_sodium_base642bin_skip_padding(const char * const b64, const size_t b64_len,
                                size_t * const b64_pos_p,
                                const char * const ignore, size_t padding_len)
{
    int c;

    while (padding_len > 0) {
        if (*b64_pos_p >= b64_len) {
            errno = ERANGE;
            return -1;
        }
        ACQUIRE_FENCE;
        c = b64[*b64_pos_p];
        if (c == '=') {
            padding_len--;
        } else if (ignore == NULL || strchr(ignore, c) == NULL) {
            errno = EINVAL;
            return -1;
        }
        (*b64_pos_p)++;
    }
    return 0;
}

int
sodium_base642bin(unsigned char * const bin, const size_t bin_maxlen,
                  const char * const b64, const size_t b64_len,
                  const char * const ignore, size_t * const bin_len,
                  const char ** const b64_end, const int variant)
{
    size_t       acc_len = (size_t) 0;
    size_t       b64_pos = (size_t) 0;
    size_t       bin_pos = (size_t) 0;
    int          is_urlsafe;
    int          ret = 0;
    unsigned int acc = 0U;
    unsigned int d;
    char         c;

    sodium_base64_check_variant(variant);
    is_urlsafe = ((unsigned int) variant) & VARIANT_URLSAFE_MASK;
    while (b64_pos < b64_len) {
        c = b64[b64_pos];
        if (is_urlsafe) {
            d = b64_urlsafe_char_to_byte(c);
        } else {
            d = b64_char_to_byte(c);
        }
        if (d == 0xFF) {
            if (ignore != NULL && strchr(ignore, c) != NULL) {
                b64_pos++;
                continue;
            }
            break;
        }
        acc = (acc << 6) + d;
        acc_len += 6;
        if (acc_len >= 8) {
            acc_len -= 8;
            if (bin_pos >= bin_maxlen) {
                errno = ERANGE;
                ret = -1;
                break;
            }
            bin[bin_pos++] = (acc >> acc_len) & 0xFF;
        }
        b64_pos++;
    }
    if (acc_len > 4U || (acc & ((1U << acc_len) - 1U)) != 0U) {
        ret = -1;
    } else if (ret == 0 &&
               (((unsigned int) variant) & VARIANT_NO_PADDING_MASK) == 0U) {
        ret = _sodium_base642bin_skip_padding(b64, b64_len, &b64_pos, ignore,
                                              acc_len / 2);
    }
    if (ret != 0) {
        bin_pos = (size_t) 0U;
    } else if (ignore != NULL) {
        while (b64_pos < b64_len && strchr(ignore, b64[b64_pos]) != NULL) {
            b64_pos++;
        }
    }
    if (b64_end != NULL) {
        *b64_end = &b64[b64_pos];
    } else if (b64_pos != b64_len) {
        errno = EINVAL;
        ret = -1;
    }
    if (bin_len != NULL) {
        *bin_len = bin_pos;
    }
    return ret;
}

static int
ip_hex_digit(int ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if (((unsigned int) ch | 32U) >= 'a' && ((unsigned int) ch | 32U) <= 'f') {
        return ((unsigned int) ch | 32U) - 'a' + 10;
    }
    return -1;
}

static int
parse_ipv4(const char *src, const char *end, unsigned char out[4])
{
    const char *p = src;
    int         i;

    if (src == NULL || end == NULL || out == NULL || src >= end) {
        return 0;
    }
    for (i = 0; i < 4; i++) {
        unsigned int val    = 0U;
        int          digits = 0;

        while (p < end && *p >= '0' && *p <= '9') {
            val = val * 10U + (unsigned int) (*p++ - '0');
            if (++digits > 3 || val > 255U) {
                return 0;
            }
        }
        if (digits == 0) {
            return 0;
        }
        out[i] = (unsigned char) val;

        if (i < 3) {
            if (p >= end || *p++ != '.') {
                return 0;
            }
        }
    }
    return p == end;
}

static int
parse_ipv6(const char *src, const char *end, unsigned char out[16])
{
    unsigned char  tmp[16]    = { 0 };
    unsigned char *tp         = tmp;
    unsigned char *endp       = tmp + 16;
    unsigned char *colonp     = NULL;
    const char    *p          = src;
    const char    *curtok     = src;
    unsigned int   val        = 0U;
    int            saw_xdigit = 0;
    int            xdigits    = 0;
    int            ch;
    int            hv;

    if (src == NULL || end == NULL || out == NULL || src >= end) {
        return 0;
    }
    if (*p == ':') {
        if (++p >= end || *p != ':') {
            return 0;
        }
        colonp = tp;
        curtok = ++p;
    }
    while (p < end) {
        ch = *p;

        if (ch == ':') {
            if (!saw_xdigit) {
                if (colonp != NULL) {
                    return 0;
                }
                colonp = tp;
                curtok = ++p;
                continue;
            }
            if (tp + 2 > endp) {
                return 0;
            }
            *tp++      = (unsigned char) (val >> 8);
            *tp++      = (unsigned char) (val & 0xffU);
            val        = 0U;
            saw_xdigit = 0;
            xdigits    = 0;
            curtok     = ++p;
            if (p >= end) {
                return 0;
            }
            continue;
        }
        if (ch == '.') {
            if (tp + 4 > endp || parse_ipv4(curtok, end, tp) == 0) {
                return 0;
            }
            tp += 4;
            saw_xdigit = 0;
            break;
        }
        hv = ip_hex_digit(ch);
        if (hv < 0 || xdigits >= 4) {
            return 0;
        }
        val        = (val << 4) | (unsigned int) hv;
        saw_xdigit = 1;
        xdigits++;
        p++;
    }
    if (saw_xdigit) {
        if (tp + 2 > endp) {
            return 0;
        }
        *tp++ = (unsigned char) (val >> 8);
        *tp++ = (unsigned char) (val & 0xffU);
    }
    if (colonp != NULL) {
        size_t n = (size_t) (tp - colonp);

        if (tp == endp) {
            return 0;
        }
        memmove(endp - n, colonp, n);
        memset(colonp, 0, (size_t) (endp - n - colonp));
        tp = endp;
    }
    if (tp != endp) {
        return 0;
    }
    memcpy(out, tmp, 16U);

    return 1;
}

int
sodium_ip2bin(unsigned char bin[16], const char *ip, size_t ip_len)
{
    const char   *ip_end = ip + ip_len;
    const char   *end;
    const char   *z;
    unsigned char v4[4];

    for (end = ip; end < ip_end && *end != 0 && *end != '%'; end++) {
        /* empty */
    }
    if (end < ip_end && *end == '%') {
        for (z = end + 1; z < ip_end && *z != 0; z++) {
            if (isspace((unsigned char) *z)) {
                return -1;
            }
        }
        if (z == end + 1) {
            return -1;
        }
    }
    if (memchr(ip, ':', (size_t) (end - ip)) != NULL) {
        return parse_ipv6(ip, end, bin) != 0 ? 0 : -1;
    }
    if (end < ip_end && *end == '%') {
        return -1;
    }
    if (parse_ipv4(ip, end, v4) == 0) {
        return -1;
    }
    memset(bin, 0, 10U);
    bin[10] = 0xffU;
    bin[11] = 0xffU;
    memcpy(bin + 12, v4, 4U);

    return 0;
}

static const unsigned char ipv4_mapped_prefix[12] = { 0U, 0U, 0U, 0U, 0U,    0U,
                                                      0U, 0U, 0U, 0U, 0xffU, 0xffU };

static void
ip_write_num(char **p, unsigned int val, int base)
{
    char buf[4];
    int  n = 0;

    do {
        unsigned int d = val % (unsigned int) base;

        buf[n++] = (char) (d < 10U ? '0' + d : 'a' + d - 10U);
        val /= (unsigned int) base;
    } while (val != 0U);

    while (n-- > 0) {
        *(*p)++ = buf[n];
    }
}

char *
sodium_bin2ip(char *ip, size_t ip_maxlen, const unsigned char bin[16])
{
    char   buf[46];
    char  *p = buf;
    int    i;
    int    best_start = -1;
    int    best_len   = 0;
    int    cur_start  = -1;
    int    cur_len    = 0;
    size_t len;

    if (ip_maxlen <= 2U) {
        return NULL;
    }
    if (memcmp(bin, ipv4_mapped_prefix, 12U) == 0) {
        for (i = 0; i < 4; i++) {
            if (i != 0) {
                *p++ = '.';
            }
            ip_write_num(&p, (unsigned int) bin[12 + i], 10);
        }
        len = (size_t) (p - buf);
        if (len >= ip_maxlen) {
            return NULL;
        }
        memcpy(ip, buf, len + 1U);
        ip[len] = 0;

        return ip;
    }
    for (i = 0; i < 8; i++) {
        unsigned int word = ((unsigned int) bin[i * 2] << 8) | (unsigned int) bin[i * 2 + 1];

        if (word == 0U) {
            if (cur_start < 0) {
                cur_start = i;
            }
            cur_len++;
        } else {
            if (cur_len > best_len) {
                best_start = cur_start;
                best_len   = cur_len;
            }
            cur_start = -1;
            cur_len   = 0;
        }
    }
    if (cur_len > best_len) {
        best_start = cur_start;
        best_len   = cur_len;
    }
    if (best_len < 2) {
        best_start = -1;
    }
    for (i = 0; i < 8; i++) {
        if (i == best_start) {
            *p++ = ':';
            *p++ = ':';
            i += best_len - 1;
            continue;
        }
        if (i != 0 && (best_start < 0 || i != best_start + best_len)) {
            *p++ = ':';
        }
        ip_write_num(&p, ((unsigned int) bin[i * 2] << 8) | (unsigned int) bin[i * 2 + 1], 16);
    }
    len = (size_t) (p - buf);
    if (len >= ip_maxlen) {
        return NULL;
    }
    memcpy(ip, buf, len);
    ip[len] = 0;

    return ip;
}
