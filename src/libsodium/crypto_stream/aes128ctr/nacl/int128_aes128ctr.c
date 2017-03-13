#include <stdint.h>

#include "common.h"
#include "int128.h"

void
xor2(aes_uint128_t *r, const aes_uint128_t *x)
{
    r->u64[0] ^= x->u64[0];
    r->u64[1] ^= x->u64[1];
}

void
and2(aes_uint128_t *r, const aes_uint128_t *x)
{
    r->u64[0] &= x->u64[0];
    r->u64[1] &= x->u64[1];
}

void
or2(aes_uint128_t *r, const aes_uint128_t *x)
{
    r->u64[0] |= x->u64[0];
    r->u64[1] |= x->u64[1];
}

void
copy2(aes_uint128_t *r, const aes_uint128_t *x)
{
    r->u64[0] = x->u64[0];
    r->u64[1] = x->u64[1];
}

void
shufb(aes_uint128_t *r, const unsigned char *l)
{
    aes_uint128_t  t;
    uint8_t       *ct;
    uint8_t       *cr;

    copy2(&t, r);
    cr     = r->u8;
    ct     = t.u8;
    cr[0]  = ct[l[0]];
    cr[1]  = ct[l[1]];
    cr[2]  = ct[l[2]];
    cr[3]  = ct[l[3]];
    cr[4]  = ct[l[4]];
    cr[5]  = ct[l[5]];
    cr[6]  = ct[l[6]];
    cr[7]  = ct[l[7]];
    cr[8]  = ct[l[8]];
    cr[9]  = ct[l[9]];
    cr[10] = ct[l[10]];
    cr[11] = ct[l[11]];
    cr[12] = ct[l[12]];
    cr[13] = ct[l[13]];
    cr[14] = ct[l[14]];
    cr[15] = ct[l[15]];
}

void
shufd(aes_uint128_t *r, const aes_uint128_t *x, const unsigned int c)
{
    aes_uint128_t t;

    t.u32[0] = x->u32[c >> 0 & 3];
    t.u32[1] = x->u32[c >> 2 & 3];
    t.u32[2] = x->u32[c >> 4 & 3];
    t.u32[3] = x->u32[c >> 6 & 3];
    copy2(r, &t);
}

void
rshift32_littleendian(aes_uint128_t *r, const unsigned int n)
{
    unsigned char *rp = (unsigned char *) r;
    uint32_t       t;

    t = LOAD32_LE(rp);
    t >>= n;
    STORE32_LE(rp, t);
    t = LOAD32_LE(rp + 4);
    t >>= n;
    STORE32_LE(rp + 4, t);
    t = LOAD32_LE(rp + 8);
    t >>= n;
    STORE32_LE(rp + 8, t);
    t = LOAD32_LE(rp + 12);
    t >>= n;
    STORE32_LE(rp + 12, t);
}

void
rshift64_littleendian(aes_uint128_t *r, const unsigned int n)
{
    unsigned char *rp = (unsigned char *) r;
    uint64_t       t;

    t = LOAD64_LE(rp);
    t >>= n;
    STORE64_LE(rp, t);
    t = LOAD64_LE(rp + 8);
    t >>= n;
    STORE64_LE(rp + 8, t);
}

void
lshift64_littleendian(aes_uint128_t *r, const unsigned int n)
{
    unsigned char *rp = (unsigned char *) r;
    uint64_t       t;

    t = LOAD64_LE(rp);
    t <<= n;
    STORE64_LE(rp, t);
    t = LOAD64_LE(rp + 8);
    t <<= n;
    STORE64_LE(rp + 8, t);
}

void
toggle(aes_uint128_t *r)
{
    r->u64[0] ^= 0xffffffffffffffffULL;
    r->u64[1] ^= 0xffffffffffffffffULL;
}

void
xor_rcon(aes_uint128_t *r)
{
    unsigned char *rp = (unsigned char *) r;
    uint32_t       t;

    t = LOAD32_LE(rp + 12);
    t ^= 0xffffffff;
    STORE32_LE(rp + 12, t);
}

void
add_uint32_big(aes_uint128_t *r, uint32_t x)
{
    unsigned char *rp = (unsigned char *) r;
    uint32_t       t;

    t = LOAD32_LE(rp + 12);
    t += x;
    STORE32_LE(rp + 12, t);
}
