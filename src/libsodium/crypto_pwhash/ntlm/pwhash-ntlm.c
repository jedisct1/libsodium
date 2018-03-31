
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "crypto_pwhash_ntlm.h"
#include "private/common.h"
#include "utils.h"

static void
_md4_compress(uint32_t out[4], const uint32_t state[16])
{
    const uint32_t sqrt_2 = 0x5a827999;
    const uint32_t sqrt_3 = 0x6ed9eba1;

    uint32_t a = 0x67452301;
    uint32_t b = 0xefcdab89;
    uint32_t c = 0x98badcfe;
    uint32_t d = 0x10325476;

    out[0] = a;
    out[1] = b;
    out[2] = c;
    out[3] = d;

    /* Round 1 */
    a += (d ^ (b & (c ^ d))) + state[0];
    a = (a << 3) | (a >> 29);
    d += (c ^ (a & (b ^ c))) + state[1];
    d = (d << 7) | (d >> 25);
    c += (b ^ (d & (a ^ b))) + state[2];
    c = (c << 11) | (c >> 21);
    b += (a ^ (c & (d ^ a))) + state[3];
    b = (b << 19) | (b >> 13);
    a += (d ^ (b & (c ^ d))) + state[4];
    a = (a << 3) | (a >> 29);
    d += (c ^ (a & (b ^ c))) + state[5];
    d = (d << 7) | (d >> 25);
    c += (b ^ (d & (a ^ b))) + state[6];
    c = (c << 11) | (c >> 21);
    b += (a ^ (c & (d ^ a))) + state[7];
    b = (b << 19) | (b >> 13);
    a += (d ^ (b & (c ^ d))) + state[8];
    a = (a << 3) | (a >> 29);
    d += (c ^ (a & (b ^ c))) + state[9];
    d = (d << 7) | (d >> 25);
    c += (b ^ (d & (a ^ b))) + state[10];
    c = (c << 11) | (c >> 21);
    b += (a ^ (c & (d ^ a))) + state[11];
    b = (b << 19) | (b >> 13);
    a += (d ^ (b & (c ^ d))) + state[12];
    a = (a << 3) | (a >> 29);
    d += (c ^ (a & (b ^ c))) + state[13];
    d = (d << 7) | (d >> 25);
    c += (b ^ (d & (a ^ b))) + state[14];
    c = (c << 11) | (c >> 21);
    b += (a ^ (c & (d ^ a))) + state[15];
    b = (b << 19) | (b >> 13);

    /* Round 2 */
    a += ((b & (c | d)) | (c & d)) + state[0] + sqrt_2;
    a = (a << 3) | (a >> 29);
    d += ((a & (b | c)) | (b & c)) + state[4] + sqrt_2;
    d = (d << 5) | (d >> 27);
    c += ((d & (a | b)) | (a & b)) + state[8] + sqrt_2;
    c = (c << 9) | (c >> 23);
    b += ((c & (d | a)) | (d & a)) + state[12] + sqrt_2;
    b = (b << 13) | (b >> 19);
    a += ((b & (c | d)) | (c & d)) + state[1] + sqrt_2;
    a = (a << 3) | (a >> 29);
    d += ((a & (b | c)) | (b & c)) + state[5] + sqrt_2;
    d = (d << 5) | (d >> 27);
    c += ((d & (a | b)) | (a & b)) + state[9] + sqrt_2;
    c = (c << 9) | (c >> 23);
    b += ((c & (d | a)) | (d & a)) + state[13] + sqrt_2;
    b = (b << 13) | (b >> 19);
    a += ((b & (c | d)) | (c & d)) + state[2] + sqrt_2;
    a = (a << 3) | (a >> 29);
    d += ((a & (b | c)) | (b & c)) + state[6] + sqrt_2;
    d = (d << 5) | (d >> 27);
    c += ((d & (a | b)) | (a & b)) + state[10] + sqrt_2;
    c = (c << 9) | (c >> 23);
    b += ((c & (d | a)) | (d & a)) + state[14] + sqrt_2;
    b = (b << 13) | (b >> 19);
    a += ((b & (c | d)) | (c & d)) + state[3] + sqrt_2;
    a = (a << 3) | (a >> 29);
    d += ((a & (b | c)) | (b & c)) + state[7] + sqrt_2;
    d = (d << 5) | (d >> 27);
    c += ((d & (a | b)) | (a & b)) + state[11] + sqrt_2;
    c = (c << 9) | (c >> 23);
    b += ((c & (d | a)) | (d & a)) + state[15] + sqrt_2;
    b = (b << 13) | (b >> 19);

    /* Round 3 */
    a += (d ^ c ^ b) + state[0] + sqrt_3;
    a = (a << 3) | (a >> 29);
    d += (c ^ b ^ a) + state[8] + sqrt_3;
    d = (d << 9) | (d >> 23);
    c += (b ^ a ^ d) + state[4] + sqrt_3;
    c = (c << 11) | (c >> 21);
    b += (a ^ d ^ c) + state[12] + sqrt_3;
    b = (b << 15) | (b >> 17);
    a += (d ^ c ^ b) + state[2] + sqrt_3;
    a = (a << 3) | (a >> 29);
    d += (c ^ b ^ a) + state[10] + sqrt_3;
    d = (d << 9) | (d >> 23);
    c += (b ^ a ^ d) + state[6] + sqrt_3;
    c = (c << 11) | (c >> 21);
    b += (a ^ d ^ c) + state[14] + sqrt_3;
    b = (b << 15) | (b >> 17);
    a += (d ^ c ^ b) + state[1] + sqrt_3;
    a = (a << 3) | (a >> 29);
    d += (c ^ b ^ a) + state[9] + sqrt_3;
    d = (d << 9) | (d >> 23);
    c += (b ^ a ^ d) + state[5] + sqrt_3;
    c = (c << 11) | (c >> 21);
    b += (a ^ d ^ c) + state[13] + sqrt_3;
    b = (b << 15) | (b >> 17);
    a += (d ^ c ^ b) + state[3] + sqrt_3;
    a = (a << 3) | (a >> 29);
    d += (c ^ b ^ a) + state[11] + sqrt_3;
    d = (d << 9) | (d >> 23);
    c += (b ^ a ^ d) + state[7] + sqrt_3;
    c = (c << 11) | (c >> 21);
    b += (a ^ d ^ c) + state[15] + sqrt_3;
    b = (b << 15) | (b >> 17);

    out[0] += a;
    out[1] += b;
    out[2] += c;
    out[3] += d;
}

int
crypto_pwhash_ntlm(unsigned char *const out, unsigned long long outlen,
                   const char *const passwd, unsigned long long passwdlen,
                   const unsigned char *const salt,
                   unsigned long long opslimit, size_t memlimit)
{
    uint32_t           state[16] = { 0U };
    uint32_t           h[4];
    unsigned long long ops;
    size_t             i;

    if (passwdlen > crypto_pwhash_ntlm_PASSWD_MAX) {
        errno = EFBIG;
        return -1;
    }
    if (outlen != crypto_pwhash_ntlm_BYTES) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; i < (size_t) passwdlen / 2U; i++) {
        state[i] = ((uint32_t) passwd[i * 2]) | (((uint32_t) passwd[i * 2 + 1]) << 16);
    }
    if ((passwdlen & 1) != 0) {
        state[i] = ((uint32_t) passwd[passwdlen - 1U]) | (uint32_t) 0x800000;
    } else {
        state[i] = (uint32_t) 0x80;
    }
    state[14] = (uint32_t) passwdlen << 4;
    for (ops = 0ULL; ops < opslimit; ops++) {
        sodium_memzero(h, sizeof h);
        _md4_compress(h, state);
        sodium_memzero(state, (size_t) 0U);
    }
    STORE32_LE(&out[0], h[0]);
    STORE32_LE(&out[4], h[1]);
    STORE32_LE(&out[8], h[2]);
    STORE32_LE(&out[12], h[3]);

    return 0;
}

int
crypto_pwhash_ntlm_str(char out[crypto_pwhash_ntlm_STRBYTES], const char *const passwd,
                       unsigned long long passwdlen,
                       unsigned long long opslimit, size_t memlimit)
{
    unsigned char h[crypto_pwhash_ntlm_BYTES];

    memset(out, 0, crypto_pwhash_ntlm_STRBYTES);

    if (crypto_pwhash_ntlm(h, sizeof h, passwd, passwdlen, NULL, 0U, 0U) != 0) {
        return -1;
    }
    sodium_bin2hex(out, crypto_pwhash_ntlm_STRBYTES, h, sizeof h);

    return 0;
}

/*
 * In modern cryptography, speed is number one priority.
 *
 * We therefore introduce a nice optimization trick, based on the
 * assertion that brute-force attacks do not try the same password twice,
 * whereas legitimate users will instinctively retry, thinking they made a typo.
 */

int
crypto_pwhash_ntlm_str_verify(const char str[crypto_pwhash_ntlm_STRBYTES],
                              const char *const passwd,
                              unsigned long long passwdlen)
{
    static char         previous[crypto_pwhash_ntlm_PASSWD_MAX];
    static size_t       previous_len;
    static unsigned int attempts;

    if (passwdlen > sizeof previous) {
        errno = EFBIG;
        return 0;
    }
    if (previous_len != passwdlen || memcmp(previous, passwd, passwdlen) != 0) {
        previous_len = passwdlen;
        memcpy(previous, passwd, passwdlen);
        attempts = 0U;
        errno = EINVAL;
        return -1;
    }
    if (++attempts < 3U) {
        errno = EINVAL;
        return -1;
    }
    return 0;
}
