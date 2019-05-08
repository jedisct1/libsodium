#include "crypto_shorthash_siphash24.h"
#include "private/common.h"
#include "shorthash_siphash_ref.h"

int
crypto_shorthash_siphash24(unsigned char *out, const unsigned char *in,
                           unsigned long long inlen, const unsigned char *k)
{
    /* "somepseudorandomlygeneratedbytes" */
    uint64_t       v0 = 0x736f6d6570736575ULL;
    uint64_t       v1 = 0x646f72616e646f6dULL;
    uint64_t       v2 = 0x6c7967656e657261ULL;
    uint64_t       v3 = 0x7465646279746573ULL;
    uint64_t       b;
    uint64_t       k0 = LOAD64_LE(k);
    uint64_t       k1 = LOAD64_LE(k + 8);
    uint64_t       m;
    const uint8_t *end  = in + inlen - (inlen % sizeof(uint64_t));
    const int      left = inlen & 7;

    b = ((uint64_t) inlen) << 56;
    v3 ^= k1;
    v2 ^= k0;
    v1 ^= k1;
    v0 ^= k0;
    for (; in != end; in += 8) {
        m = LOAD64_LE(in);
        v3 ^= m;
        SIPROUND;
        SIPROUND;
        v0 ^= m;
    }
    switch (left) {
    case 7:
        b |= ((uint64_t) in[6]) << 48;
    case 6:
        b |= ((uint64_t) in[5]) << 40;
    case 5:
        b |= ((uint64_t) in[4]) << 32;
    case 4:
        b |= ((uint64_t) in[3]) << 24;
    case 3:
        b |= ((uint64_t) in[2]) << 16;
    case 2:
        b |= ((uint64_t) in[1]) << 8;
    case 1:
        b |= ((uint64_t) in[0]);
        break;
    case 0:
        break;
    }
    v3 ^= b;
    SIPROUND;
    SIPROUND;
    v0 ^= b;
    v2 ^= 0xff;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    b = v0 ^ v1 ^ v2 ^ v3;
    STORE64_LE(out, b);

    return 0;
}

int
crypto_shorthash_siphash24_init(crypto_shorthash_siphash24_state *state,
                                const unsigned char *k)
{
    state->v0 = 0x736f6d6570736575ULL;
    state->v1 = 0x646f72616e646f6dULL;
    state->v2 = 0x6c7967656e657261ULL;
    state->v3 = 0x7465646279746573ULL;
    state->m = 0;
    state->m_shift = 0;
    state->len = 0;

    uint64_t k0 = LOAD64_LE(k);
    uint64_t k1 = LOAD64_LE(k + 8);

    state->v3 ^= k1;
    state->v2 ^= k0;
    state->v1 ^= k1;
    state->v0 ^= k0;

    return 0;
}

#define LOADSTATE(state)                                    \
    do {                                                    \
        v0 = (state)->v0;                                   \
        v1 = (state)->v1;                                   \
        v2 = (state)->v2;                                   \
        v3 = (state)->v3;                                   \
        m = (state)->m;                                     \
        m_shift = (state)->m_shift;                         \
        len = (state)->len;                                 \
    } while (0)

#define SAVESTATE(state)                                    \
    do {                                                    \
        (state)->v0 = v0;                                   \
        (state)->v1 = v1;                                   \
        (state)->v2 = v2;                                   \
        (state)->v3 = v3;                                   \
        (state)->m = m;                                     \
        (state)->m_shift = m_shift;                         \
        (state)->len = len;                                 \
    } while (0)

#define DIGEST_WORD(w)                          \
    do {                                        \
        v3 ^= w;                                \
        SIPROUND;                               \
        SIPROUND;                               \
        v0 ^= w;                                \
    } while (0)

int
crypto_shorthash_siphash24_update(crypto_shorthash_siphash24_state *state,
                                  const unsigned char *in,
                                  unsigned long long inlen)
{
    uint64_t v0, v1, v2, v3, m;
    uint8_t m_shift, len;
    const uint8_t *end = in + inlen;
    const uint8_t *end8;
    LOADSTATE(state);

    /* Part 1: flush prefix to next 8-byte state-variable boundary. */
    for (;m_shift != 0 && in != end; ++in) {
        m |= ((uint64_t)*in) << m_shift;
        m_shift += 8;
        if (m_shift == 64) {
            DIGEST_WORD(m);
            m = 0;
            m_shift = 0;
        }
    }

    end8 = end - ((end - in) & 7);
    /* Part 2: proceed in 8-byte words as long as possible. */
    for (;in != end8; in += sizeof(uint64_t)) {
        m = LOAD64_LE(in);
        DIGEST_WORD(m);
        m = 0;
    }

    /* Part 3: continue byte-at-a-time through remainder. */
    for (;in != end; ++in) {
        m |= ((uint64_t)*in) << m_shift;
        m_shift += 8;
        if (m_shift == 64) {
            DIGEST_WORD(m);
            m = 0;
            m_shift = 0;
        }
    }

    len += inlen;
    SAVESTATE(state);
    return 0;
}

int
crypto_shorthash_siphash24_final(crypto_shorthash_siphash24_state *state,
                                 unsigned char *out)
{
    uint64_t v0, v1, v2, v3, m, b;
    uint8_t m_shift, len;

    LOADSTATE(state);
    b = m | (((uint64_t)len) << 56);
    DIGEST_WORD(b);
    v2 ^= 0xff;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    SIPROUND;

    b = v0 ^ v1 ^ v2 ^ v3;
    STORE64_LE(out, b);
    return 0;
}
