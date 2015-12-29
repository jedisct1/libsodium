#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "argon2-encoding.h"

/*
 * Example code for a decoder and encoder of "hash strings", with Argon2i
 * parameters.
 *
 * This code comprises three sections:
 *
 *   -- The first section contains generic Base64 encoding and decoding
 *   functions. It is conceptually applicable to any hash function
 *   implementation that uses Base64 to encode and decode parameters,
 *   salts and outputs. It could be made into a library, provided that
 *   the relevant functions are made public (non-static) and be given
 *   reasonable names to avoid collisions with other functions.
 *
 *   -- The second section is specific to Argon2i. It encodes and decodes
 *   the parameters, salts and outputs. It does not compute the hash
 *   itself.
 *
 *   -- The third section is test code, with a main() function. With
 *   this section, the whole file compiles as a stand-alone program
 *   that exercises the encoding and decoding functions with some
 *   test vectors.
 *
 * The code was originally written by Thomas Pornin <pornin@bolet.org>,
 * to whom comments and remarks may be sent. It is released under what
 * should amount to Public Domain or its closest equivalent; the
 * following mantra is supposed to incarnate that fact with all the
 * proper legal rituals:
 *
 * ---------------------------------------------------------------------
 * This file is provided under the terms of Creative Commons CC0 1.0
 * Public Domain Dedication. To the extent possible under law, the
 * author (Thomas Pornin) has waived all copyright and related or
 * neighboring rights to this file. This work is published from: Canada.
 * ---------------------------------------------------------------------
 *
 * Copyright (c) 2015 Thomas Pornin
 */

/* ==================================================================== */
/*
 * Common code; could be shared between different hash functions.
 *
 * Note: the Base64 functions below assume that uppercase letters (resp.
 * lowercase letters) have consecutive numerical codes, that fit on 8
 * bits. All modern systems use ASCII-compatible charsets, where these
 * properties are true. If you are stuck with a dinosaur of a system
 * that still defaults to EBCDIC then you already have much bigger
 * interoperability issues to deal with.
 */

/*
 * Some macros for constant-time comparisons. These work over values in
 * the 0..255 range. Returned value is 0x00 on "false", 0xFF on "true".
 */
#define EQ(x, y) ((((0U-((unsigned)(x) ^ (unsigned)(y))) >> 8) & 0xFF) ^ 0xFF)
#define GT(x, y) ((((unsigned)(y) - (unsigned)(x)) >> 8) & 0xFF)
#define GE(x, y) (GT(y, x) ^ 0xFF)
#define LT(x, y) GT(y, x)
#define LE(x, y) GE(y, x)

/*
 * Convert value x (0..63) to corresponding Base64 character.
 */
static int b64_byte_to_char(unsigned x) {
    return (LT(x, 26) & (x + 'A')) |
           (GE(x, 26) & LT(x, 52) & (x + ('a' - 26))) |
           (GE(x, 52) & LT(x, 62) & (x + ('0' - 52))) | (EQ(x, 62) & '+') |
           (EQ(x, 63) & '/');
}

/*
 * Convert character c to the corresponding 6-bit value. If character c
 * is not a Base64 character, then 0xFF (255) is returned.
 */
static unsigned b64_char_to_byte(int c) {
    unsigned x;

    x = (GE(c, 'A') & LE(c, 'Z') & (c - 'A')) |
        (GE(c, 'a') & LE(c, 'z') & (c - ('a' - 26))) |
        (GE(c, '0') & LE(c, '9') & (c - ('0' - 52))) | (EQ(c, '+') & 62) |
        (EQ(c, '/') & 63);
    return x | (EQ(x, 0) & (EQ(c, 'A') ^ 0xFF));
}

/*
 * Convert some bytes to Base64. 'dst_len' is the length (in characters)
 * of the output buffer 'dst'; if that buffer is not large enough to
 * receive the result (including the terminating 0), then (size_t)-1
 * is returned. Otherwise, the zero-terminated Base64 string is written
 * in the buffer, and the output length (counted WITHOUT the terminating
 * zero) is returned.
 */
static size_t to_base64(char *dst, size_t dst_len, const void *src,
                        size_t src_len) {
    size_t olen;
    const unsigned char *buf;
    unsigned acc, acc_len;

    olen = (src_len / 3) << 2;
    switch (src_len % 3) {
    case 2:
        olen++;
    /* fall through */
    case 1:
        olen += 2;
        break;
    }
    if (dst_len <= olen) {
        return (size_t)-1;
    }
    acc = 0;
    acc_len = 0;
    buf = (const unsigned char *)src;
    while (src_len-- > 0) {
        acc = (acc << 8) + (*buf++);
        acc_len += 8;
        while (acc_len >= 6) {
            acc_len -= 6;
            *dst++ = (char) b64_byte_to_char((acc >> acc_len) & 0x3F);
        }
    }
    if (acc_len > 0) {
        *dst++ = (char) b64_byte_to_char((acc << (6 - acc_len)) & 0x3F);
    }
    *dst++ = 0;
    return olen;
}

/*
 * Decode Base64 chars into bytes. The '*dst_len' value must initially
 * contain the length of the output buffer '*dst'; when the decoding
 * ends, the actual number of decoded bytes is written back in
 * '*dst_len'.
 *
 * Decoding stops when a non-Base64 character is encountered, or when
 * the output buffer capacity is exceeded. If an error occurred (output
 * buffer is too small, invalid last characters leading to unprocessed
 * buffered bits), then NULL is returned; otherwise, the returned value
 * points to the first non-Base64 character in the source stream, which
 * may be the terminating zero.
 */
static const char *from_base64(void *dst, size_t *dst_len, const char *src) {
    size_t len;
    unsigned char *buf;
    unsigned acc, acc_len;

    buf = (unsigned char *)dst;
    len = 0;
    acc = 0;
    acc_len = 0;
    for (;;) {
        unsigned d;

        d = b64_char_to_byte(*src);
        if (d == 0xFF) {
            break;
        }
        src++;
        acc = (acc << 6) + d;
        acc_len += 6;
        if (acc_len >= 8) {
            acc_len -= 8;
            if ((len++) >= *dst_len) {
                return NULL;
            }
            *buf++ = (acc >> acc_len) & 0xFF;
        }
    }

    /*
     * If the input length is equal to 1 modulo 4 (which is
     * invalid), then there will remain 6 unprocessed bits;
     * otherwise, only 0, 2 or 4 bits are buffered. The buffered
     * bits must also all be zero.
     */
    if (acc_len > 4 || (acc & (((unsigned)1 << acc_len) - 1)) != 0) {
        return NULL;
    }
    *dst_len = len;
    return src;
}

/*
 * Decode decimal integer from 'str'; the value is written in '*v'.
 * Returned value is a pointer to the next non-decimal character in the
 * string. If there is no digit at all, or the value encoding is not
 * minimal (extra leading zeros), or the value does not fit in an
 * 'unsigned long', then NULL is returned.
 */
static const char *decode_decimal(const char *str, unsigned long *v) {
    const char *orig;
    unsigned long acc;

    acc = 0;
    for (orig = str;; str++) {
        int c;

        c = *str;
        if (c < '0' || c > '9') {
            break;
        }
        c -= '0';
        if (acc > (ULONG_MAX / 10)) {
            return NULL;
        }
        acc *= 10;
        if ((unsigned long)c > (ULONG_MAX - acc)) {
            return NULL;
        }
        acc += (unsigned long)c;
    }
    if (str == orig || (*orig == '0' && str != (orig + 1))) {
        return NULL;
    }
    *v = acc;
    return str;
}

/* ==================================================================== */
/*
 * Code specific to Argon2i.
 *
 * The code below applies the following format:
 *
 *  $argon2i$m=<num>,t=<num>,p=<num>[,keyid=<bin>][,data=<bin>][$<bin>[$<bin>]]
 *
 * where <num> is a decimal integer (positive, fits in an 'unsigned long')
 * and <bin> is Base64-encoded data (no '=' padding characters, no newline
 * or whitespace). The "keyid" is a binary identifier for a key (up to 8
 * bytes); "data" is associated data (up to 32 bytes). When the 'keyid'
 * (resp. the 'data') is empty, then it is ommitted from the output.
 *
 * The last two binary chunks (encoded in Base64) are, in that order,
 * the salt and the output. Both are optional, but you cannot have an
 * output without a salt. The binary salt length is between 8 and 48 bytes.
 * The output length is always exactly 32 bytes.
 */

/*
 * Decode an Argon2i hash string into the provided structure 'ctx'.
 * Returned value is 1 on success, 0 on error.
 */
int decode_string(argon2_context *ctx, const char *str, argon2_type type) {
#define CC(prefix)                                                             \
    do {                                                                       \
        size_t cc_len = strlen(prefix);                                        \
        if (strncmp(str, prefix, cc_len) != 0) {                               \
            return 0;                                                          \
        }                                                                      \
        str += cc_len;                                                         \
    } while ((void)0, 0)

#define CC_opt(prefix, code)                                                   \
    do {                                                                       \
        size_t cc_len = strlen(prefix);                                        \
        if (strncmp(str, prefix, cc_len) == 0) {                               \
            str += cc_len;                                                     \
            { code; }                                                          \
        }                                                                      \
    } while ((void)0, 0)

#define DECIMAL(x)                                                             \
    do {                                                                       \
        unsigned long dec_x;                                                   \
        str = decode_decimal(str, &dec_x);                                     \
        if (str == NULL) {                                                     \
            return 0;                                                          \
        }                                                                      \
        (x) = dec_x;                                                           \
    } while ((void)0, 0)

#define BIN(buf, max_len, len)                                                 \
    do {                                                                       \
        size_t bin_len = (max_len);                                            \
        str = from_base64(buf, &bin_len, str);                                 \
        if (str == NULL || bin_len > UINT32_MAX) {                             \
            return 0;                                                          \
        }                                                                      \
        (len) = (uint32_t)bin_len;                                             \
    } while ((void)0, 0)

    size_t maxadlen = ctx->adlen;
    size_t maxsaltlen = ctx->saltlen;
    size_t maxoutlen = ctx->outlen;

    ctx->adlen = 0;
    ctx->saltlen = 0;
    ctx->outlen = 0;
    if (type == Argon2_i)
        CC("$argon2i");
    else
        return 0;
    CC("$m=");
    DECIMAL(ctx->m_cost);
    CC(",t=");
    DECIMAL(ctx->t_cost);
    CC(",p=");
    DECIMAL(ctx->lanes);
    ctx->threads = ctx->lanes;

    /*
     * Both m and t must be no more than 2^32-1. The tests below
     * use a shift by 30 bits to avoid a direct comparison with
     * 0xFFFFFFFF, which may trigger a spurious compiler warning
     * on machines where 'unsigned long' is a 32-bit type.
     */
    if (ctx->m_cost < 1 || (ctx->m_cost >> 30) > 3) {
        return 0;
    }
    if (ctx->t_cost < 1 || (ctx->t_cost >> 30) > 3) {
        return 0;
    }

    /*
     * The parallelism p must be between 1 and 255. The memory cost
     * parameter, expressed in kilobytes, must be at least 8 times
     * the value of p.
     */
    if (ctx->lanes < 1 || ctx->lanes > 255) {
        return 0;
    }
    if (ctx->m_cost < (ctx->lanes << 3)) {
        return 0;
    }

    CC_opt(",data=", BIN(ctx->ad, maxadlen, ctx->adlen));
    if (*str == 0) {
        return 1;
    }
    CC("$");
    BIN(ctx->salt, maxsaltlen, ctx->saltlen);
    if (ctx->saltlen < 8) {
        return 0;
    }
    if (*str == 0) {
        return 1;
    }
    CC("$");
    BIN(ctx->out, maxoutlen, ctx->outlen);
    if (ctx->outlen < 12) {
        return 0;
    }
    return *str == 0;

#undef CC
#undef CC_opt
#undef DECIMAL
#undef BIN
}

#define U32_STR_MAXSIZE 11U

static void u32_to_string(char *str, uint32_t x) {
    char   tmp[U32_STR_MAXSIZE - 1U];
    size_t i;

    i = sizeof tmp;
    do {
        tmp[--i] = (x % (uint32_t) 10U) + '0';
        x /= (uint32_t) 10U;
    } while (x != 0U && i != 0U);
    memcpy(str, &tmp[i], (sizeof tmp) - i);
    str[(sizeof tmp) - i] = 0;
}

/*
 * encode an argon2i hash string into the provided buffer. 'dst_len'
 * contains the size, in characters, of the 'dst' buffer; if 'dst_len'
 * is less than the number of required characters (including the
 * terminating 0), then this function returns 0.
 *
 * if pp->output_len is 0, then the hash string will be a salt string
 * (no output). if pp->salt_len is also 0, then the string will be a
 * parameter-only string (no salt and no output).
 *
 * on success, 1 is returned.
 */
int encode_string(char *dst, size_t dst_len, argon2_context *ctx,
                  argon2_type type) {
#define SS(str)                                                                \
    do {                                                                       \
        size_t pp_len = strlen(str);                                           \
        if (pp_len >= dst_len) {                                               \
            return 0;                                                          \
        }                                                                      \
        memcpy(dst, str, pp_len + 1);                                          \
        dst += pp_len;                                                         \
        dst_len -= pp_len;                                                     \
    } while ((void)0, 0)

#define SX(x)                                                                  \
    do {                                                                       \
        char tmp[U32_STR_MAXSIZE];                                             \
        u32_to_string(tmp, x);                                                 \
        SS(tmp);                                                               \
    } while ((void)0, 0)

#define SB(buf, len)                                                           \
    do {                                                                       \
        size_t sb_len = to_base64(dst, dst_len, buf, len);                     \
        if (sb_len == (size_t)-1) {                                            \
            return 0;                                                          \
        }                                                                      \
        dst += sb_len;                                                         \
        dst_len -= sb_len;                                                     \
    } while ((void)0, 0)

    if (type == Argon2_i)
        SS("$argon2i$m=");
    else
        return 0;
    SX(ctx->m_cost);
    SS(",t=");
    SX(ctx->t_cost);
    SS(",p=");
    SX(ctx->lanes);

    if (ctx->adlen > 0) {
        SS(",data=");
        SB(ctx->ad, ctx->adlen);
    }

    if (ctx->saltlen == 0)
        return 1;

    SS("$");
    SB(ctx->salt, ctx->saltlen);

    if (ctx->outlen == 0)
        return 1;

    SS("$");
    SB(ctx->out, ctx->outlen);
    return 1;

#undef SS
#undef SX
#undef SB
}
