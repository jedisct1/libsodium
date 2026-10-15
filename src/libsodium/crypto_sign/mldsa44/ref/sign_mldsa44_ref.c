#include <stdint.h>
#include <string.h>

#include "crypto_sign_mldsa44.h"
#include "crypto_verify_32.h"
#include "crypto_xof_shake128.h"
#include "crypto_xof_shake256.h"
#include "private/common.h"
#include "randombytes.h"
#include "sign_mldsa44_ref.h"
#include "utils.h"

#define MLDSA44_Q      8380417
#define MLDSA44_QINV   4236238847U
#define MLDSA44_N      256
#define MLDSA44_K      4
#define MLDSA44_L      4
#define MLDSA44_D      13
#define MLDSA44_ETA    2
#define MLDSA44_TAU    39
#define MLDSA44_BETA   78
#define MLDSA44_GAMMA1 (1 << 17)
#define MLDSA44_GAMMA2 ((MLDSA44_Q - 1) / 88)
#define MLDSA44_OMEGA  80

#define MLDSA44_SEEDBYTES   32
#define MLDSA44_RNDBYTES    32
#define MLDSA44_CRHBYTES    64
#define MLDSA44_TRBYTES     64
#define MLDSA44_CTILDEBYTES 32

#define MLDSA44_POLYETA_PACKEDBYTES  96
#define MLDSA44_POLYT0_PACKEDBYTES   416
#define MLDSA44_POLYT1_PACKEDBYTES   320
#define MLDSA44_POLYZ_PACKEDBYTES    576
#define MLDSA44_POLYW1_PACKEDBYTES   192
#define MLDSA44_POLYVECH_PACKEDBYTES (MLDSA44_OMEGA + MLDSA44_K)

#define MLDSA44_SK_KEY_OFFSET MLDSA44_SEEDBYTES
#define MLDSA44_SK_TR_OFFSET  (MLDSA44_SK_KEY_OFFSET + MLDSA44_SEEDBYTES)
#define MLDSA44_SK_S1_OFFSET  (MLDSA44_SK_TR_OFFSET + MLDSA44_TRBYTES)
#define MLDSA44_SK_S2_OFFSET  (MLDSA44_SK_S1_OFFSET + MLDSA44_L * MLDSA44_POLYETA_PACKEDBYTES)
#define MLDSA44_SK_T0_OFFSET  (MLDSA44_SK_S2_OFFSET + MLDSA44_K * MLDSA44_POLYETA_PACKEDBYTES)
#define MLDSA44_SIG_Z_OFFSET  MLDSA44_CTILDEBYTES
#define MLDSA44_SIG_H_OFFSET  (MLDSA44_SIG_Z_OFFSET + MLDSA44_L * MLDSA44_POLYZ_PACKEDBYTES)

#define MLDSA44_SIGN_MAX_ITERATIONS 821

typedef struct poly {
    uint32_t coeffs[MLDSA44_N];
} poly;

typedef struct polyvec {
    poly vec[MLDSA44_K];
} polyvec;

static const uint32_t zetas[256] = {
    4193792, 25847,   5771523, 7861508, 237124,  7602457, 7504169, 466468,
    1826347, 2353451, 8021166, 6288512, 3119733, 5495562, 3111497, 2680103,
    2725464, 1024112, 7300517, 3585928, 7830929, 7260833, 2619752, 6271868,
    6262231, 4520680, 6980856, 5102745, 1757237, 8360995, 4010497, 280005,
    2706023, 95776,   3077325, 3530437, 6718724, 4788269, 5842901, 3915439,
    4519302, 5336701, 3574422, 5512770, 3539968, 8079950, 2348700, 7841118,
    6681150, 6736599, 3505694, 4558682, 3507263, 6239768, 6779997, 3699596,
    811944,  531354,  954230,  3881043, 3900724, 5823537, 2071892, 5582638,
    4450022, 6851714, 4702672, 5339162, 6927966, 3475950, 2176455, 6795196,
    7122806, 1939314, 4296819, 7380215, 5190273, 5223087, 4747489, 126922,
    3412210, 7396998, 2147896, 2715295, 5412772, 4686924, 7969390, 5903370,
    7709315, 7151892, 8357436, 7072248, 7998430, 1349076, 1852771, 6949987,
    5037034, 264944,  508951,  3097992, 44288,   7280319, 904516,  3958618,
    4656075, 8371839, 1653064, 5130689, 2389356, 8169440, 759969,  7063561,
    189548,  4827145, 3159746, 6529015, 5971092, 8202977, 1315589, 1341330,
    1285669, 6795489, 7567685, 6940675, 5361315, 4499357, 4751448, 3839961,
    2091667, 3407706, 2316500, 3817976, 5037939, 2244091, 5933984, 4817955,
    266997,  2434439, 7144689, 3513181, 4860065, 4621053, 7183191, 5187039,
    900702,  1859098, 909542,  819034,  495491,  6767243, 8337157, 7857917,
    7725090, 5257975, 2031748, 3207046, 4823422, 7855319, 7611795, 4784579,
    342297,  286988,  5942594, 4108315, 3437287, 5038140, 1735879, 203044,
    2842341, 2691481, 5790267, 1265009, 4055324, 1247620, 2486353, 1595974,
    4613401, 1250494, 2635921, 4832145, 5386378, 1869119, 1903435, 7329447,
    7047359, 1237275, 5062207, 6950192, 7929317, 1312455, 3306115, 6417775,
    7100756, 1917081, 5834105, 7005614, 1500165, 777191,  2235880, 3406031,
    7838005, 5548557, 6709241, 6533464, 5796124, 4656147, 594136,  4603424,
    6366809, 2432395, 2454455, 8215696, 1957272, 3369112, 185531,  7173032,
    5196991, 162844,  1616392, 3014001, 810149,  1652634, 4686184, 6581310,
    5341501, 3523897, 3866901, 269760,  2213111, 7404533, 1717735, 472078,
    7953734, 1723600, 6577327, 1910376, 6712985, 7276084, 8119771, 4546524,
    5441381, 6144432, 7959518, 6094090, 183443,  7403526, 1612842, 4834730,
    7826001, 3919660, 8332111, 7018208, 3937738, 1400424, 7534263, 1976782
};

static uint32_t
montgomery_reduce(uint64_t a)
{
    uint32_t t;

    t = (uint32_t) a * MLDSA44_QINV;
    t = (uint32_t) ((a + (uint64_t) t * MLDSA44_Q) >> 32);

    return t;
}

static uint32_t
reduce_le2q(uint32_t a)
{
    uint32_t t;

    t = a >> 23;
    t = (a & 0x7FFFFF) + (t << 13) - t;

    return t;
}

static uint32_t
csubq(uint32_t a)
{
    a -= MLDSA44_Q;
    a += (0U - (a >> 31)) & MLDSA44_Q;

    return a;
}

static void
poly_ntt(poly *r)
{
    unsigned int len, start, j, k;
    uint32_t     t, zeta;

    k = 1;
    for (len = 128; len > 0; len >>= 1) {
        for (start = 0; start < MLDSA44_N; start = j + len) {
            zeta = zetas[k++];
            for (j = start; j < start + len; j++) {
                t                  = montgomery_reduce((uint64_t) zeta * r->coeffs[j + len]);
                r->coeffs[j + len] = r->coeffs[j] + 2 * MLDSA44_Q - t;
                r->coeffs[j]       = r->coeffs[j] + t;
            }
        }
    }
}

static void
poly_invntt(poly *r)
{
    unsigned int   start, len, j, k;
    uint32_t       t, zeta;
    const uint32_t f = 41978;

    k = 255;
    for (len = 1; len <= 128; len <<= 1) {
        for (start = 0; start < MLDSA44_N; start = j + len) {
            zeta = MLDSA44_Q - zetas[k--];
            for (j = start; j < start + len; j++) {
                t                  = r->coeffs[j];
                r->coeffs[j]       = t + r->coeffs[j + len];
                r->coeffs[j + len] = montgomery_reduce(
                    (uint64_t) zeta * (t + 256U * MLDSA44_Q - r->coeffs[j + len]));
            }
        }
    }
    for (j = 0; j < MLDSA44_N; j++) {
        r->coeffs[j] = montgomery_reduce((uint64_t) f * r->coeffs[j]);
    }
}

static void
poly_basemul(poly *r, const poly *a, const poly *b)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_N; i++) {
        r->coeffs[i] = montgomery_reduce((uint64_t) a->coeffs[i] * b->coeffs[i]);
    }
}

static void
poly_reduce(poly *r)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_N; i++) {
        r->coeffs[i] = reduce_le2q(r->coeffs[i]);
    }
}

static void
poly_add(poly *r, const poly *a, const poly *b)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_N; i++) {
        r->coeffs[i] = a->coeffs[i] + b->coeffs[i];
    }
}

static void
poly_sub(poly *r, const poly *a, const poly *b)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_N; i++) {
        r->coeffs[i] = a->coeffs[i] + 2 * MLDSA44_Q - b->coeffs[i];
    }
}

static void
poly_csubq(poly *r)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_N; i++) {
        r->coeffs[i] = csubq(r->coeffs[i]);
    }
}

static void
poly_shiftl(poly *r)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_N; i++) {
        r->coeffs[i] <<= MLDSA44_D;
    }
}

static uint32_t
poly_chknorm(const poly *a, uint32_t bound)
{
    unsigned int i;
    uint32_t     t, r = 0;

    for (i = 0; i < MLDSA44_N; i++) {
        t = (MLDSA44_Q - 1) / 2 - a->coeffs[i];
        t ^= 0U - (t >> 31);
        t = (MLDSA44_Q - 1) / 2 - t;
        r |= (bound - 1 - t) >> 31;
    }

    return r;
}

static uint32_t
power2round(uint32_t *a0, uint32_t a)
{
    uint32_t t;

    t = a & ((1U << MLDSA44_D) - 1);
    t -= (1U << (MLDSA44_D - 1)) + 1;
    t += (0U - (t >> 31)) & (1U << MLDSA44_D);
    t -= (1U << (MLDSA44_D - 1)) - 1;
    *a0 = MLDSA44_Q + t;

    return (a - t) >> MLDSA44_D;
}

static uint32_t
decompose(uint32_t *a0, uint32_t a)
{
    uint32_t a1;

    a1 = (a + 127) >> 7;
    a1 = (a1 * 11275 + (1U << 23)) >> 24;
    a1 ^= (0U - ((43 - a1) >> 31)) & a1;
    *a0 = a - a1 * 2 * MLDSA44_GAMMA2;
    *a0 += (0U - ((*a0 - (MLDSA44_Q - 1) / 2) >> 31)) & MLDSA44_Q;

    return a1;
}

static uint32_t
make_hint(uint32_t a0, uint32_t a1)
{
    uint32_t t, low;

    low = 1 ^ ((MLDSA44_GAMMA2 - a0) >> 31);
    low |= (MLDSA44_Q - MLDSA44_GAMMA2 - a0) >> 31;
    t = a0 ^ (MLDSA44_Q - MLDSA44_GAMMA2);
    t = 1 ^ ((t | (0U - t)) >> 31);
    t &= 1 ^ ((a1 | (0U - a1)) >> 31);
    low |= t;

    return 1 ^ low;
}

static uint32_t
use_hint(uint32_t a, uint32_t hint)
{
    uint32_t a0, a1;

    a1 = decompose(&a0, a);
    if (hint == 0) {
        return a1;
    }
    if (a0 > MLDSA44_Q) {
        return (a1 == 43) ? 0 : a1 + 1;
    }
    return (a1 == 0) ? 43 : a1 - 1;
}

static void
poly_power2round(poly *a1, poly *a0, const poly *a)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_N; i++) {
        a1->coeffs[i] = power2round(&a0->coeffs[i], a->coeffs[i]);
    }
}

static void
poly_decompose(poly *a1, poly *a0, const poly *a)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_N; i++) {
        a1->coeffs[i] = decompose(&a0->coeffs[i], a->coeffs[i]);
    }
}

static unsigned int
poly_make_hint(poly *h, const poly *a0, const poly *a1)
{
    unsigned int i;
    unsigned int s = 0;

    for (i = 0; i < MLDSA44_N; i++) {
        h->coeffs[i] = make_hint(a0->coeffs[i], a1->coeffs[i]);
        s += h->coeffs[i];
    }

    return s;
}

static void
poly_use_hint(poly *r, const poly *a, const poly *h)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_N; i++) {
        r->coeffs[i] = use_hint(a->coeffs[i], h->coeffs[i]);
    }
}

static unsigned int
rej_eta(uint32_t *r, unsigned int len, const unsigned char *buf, unsigned int buflen)
{
    unsigned int ctr, pos;
    uint32_t     t0, t1;

    ctr = pos = 0;
    while (ctr < len && pos < buflen) {
        t0 = buf[pos] & 0x0F;
        t1 = buf[pos++] >> 4;

        if (t0 < 15) {
            t0       = t0 - ((205 * t0) >> 10) * 5;
            r[ctr++] = MLDSA44_Q + MLDSA44_ETA - t0;
        }
        if (t1 < 15 && ctr < len) {
            t1       = t1 - ((205 * t1) >> 10) * 5;
            r[ctr++] = MLDSA44_Q + MLDSA44_ETA - t1;
        }
    }

    return ctr;
}

static void
poly_getnoise_eta(poly *r, const unsigned char seed[MLDSA44_CRHBYTES], uint16_t nonce)
{
    unsigned char             buf[crypto_xof_shake256_BLOCKBYTES];
    crypto_xof_shake256_state state;
    unsigned char             extseed[MLDSA44_CRHBYTES + 2];
    unsigned int              ctr;

    memcpy(extseed, seed, MLDSA44_CRHBYTES);
    extseed[MLDSA44_CRHBYTES]     = (unsigned char) nonce;
    extseed[MLDSA44_CRHBYTES + 1] = (unsigned char) (nonce >> 8);

    crypto_xof_shake256_init(&state);
    crypto_xof_shake256_update(&state, extseed, sizeof extseed);
    crypto_xof_shake256_squeeze(&state, buf, sizeof buf);

    ctr = rej_eta(r->coeffs, MLDSA44_N, buf, sizeof buf);
    while (ctr < MLDSA44_N) {
        crypto_xof_shake256_squeeze(&state, buf, sizeof buf);
        ctr += rej_eta(r->coeffs + ctr, MLDSA44_N - ctr, buf, sizeof buf);
    }
    sodium_memzero(&state, sizeof state);
    sodium_memzero(buf, sizeof buf);
    sodium_memzero(extseed, sizeof extseed);
}

static void
poly_challenge(poly *c, const unsigned char seed[MLDSA44_CTILDEBYTES])
{
    unsigned char             buf[crypto_xof_shake256_BLOCKBYTES];
    crypto_xof_shake256_state state;
    uint64_t                  signs;
    unsigned int              i, b, pos;

    crypto_xof_shake256_init(&state);
    crypto_xof_shake256_update(&state, seed, MLDSA44_CTILDEBYTES);
    crypto_xof_shake256_squeeze(&state, buf, sizeof buf);

    signs = LOAD64_LE(buf);
    pos   = 8;

    memset(c, 0, sizeof *c);
    for (i = MLDSA44_N - MLDSA44_TAU; i < MLDSA44_N; i++) {
        do {
            if (pos >= sizeof buf) {
                crypto_xof_shake256_squeeze(&state, buf, sizeof buf);
                pos = 0;
            }
            b = buf[pos++];
        } while (b > i);

        c->coeffs[i] = c->coeffs[b];
        c->coeffs[b] = 1 + ((0U - (uint32_t) (signs & 1)) & (MLDSA44_Q - 2));
        signs >>= 1;
    }
    sodium_memzero(&state, sizeof state);
    sodium_memzero(buf, sizeof buf);
}

static void
poly_pack_eta(unsigned char r[MLDSA44_POLYETA_PACKEDBYTES], const poly *a)
{
    unsigned int i, j;
    uint32_t     t[8];

    for (i = 0; i < MLDSA44_N / 8; i++) {
        for (j = 0; j < 8; j++) {
            t[j] = MLDSA44_Q + MLDSA44_ETA - a->coeffs[8 * i + j];
        }

        r[3 * i + 0] = (unsigned char) ((t[0] >> 0) | (t[1] << 3) | (t[2] << 6));
        r[3 * i + 1] = (unsigned char) ((t[2] >> 2) | (t[3] << 1) | (t[4] << 4) | (t[5] << 7));
        r[3 * i + 2] = (unsigned char) ((t[5] >> 1) | (t[6] << 2) | (t[7] << 5));
    }
}

static uint32_t
poly_unpack_eta(poly *r, const unsigned char a[MLDSA44_POLYETA_PACKEDBYTES])
{
    unsigned int i, j;
    uint32_t     t[8];
    uint32_t     bad = 0;

    for (i = 0; i < MLDSA44_N / 8; i++) {
        t[0] = (a[3 * i + 0] >> 0) & 7;
        t[1] = (a[3 * i + 0] >> 3) & 7;
        t[2] = ((a[3 * i + 0] >> 6) | ((uint32_t) a[3 * i + 1] << 2)) & 7;
        t[3] = (a[3 * i + 1] >> 1) & 7;
        t[4] = (a[3 * i + 1] >> 4) & 7;
        t[5] = ((a[3 * i + 1] >> 7) | ((uint32_t) a[3 * i + 2] << 1)) & 7;
        t[6] = (a[3 * i + 2] >> 2) & 7;
        t[7] = (a[3 * i + 2] >> 5) & 7;

        for (j = 0; j < 8; j++) {
            bad |= (2 * MLDSA44_ETA - t[j]) >> 31;
            r->coeffs[8 * i + j] = MLDSA44_Q + MLDSA44_ETA - t[j];
        }
    }

    return bad;
}

static void
poly_pack_t1(unsigned char r[MLDSA44_POLYT1_PACKEDBYTES], const poly *a)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_N / 4; i++) {
        r[5 * i + 0] = (unsigned char) (a->coeffs[4 * i + 0] >> 0);
        r[5 * i + 1] = (unsigned char) ((a->coeffs[4 * i + 0] >> 8) | (a->coeffs[4 * i + 1] << 2));
        r[5 * i + 2] = (unsigned char) ((a->coeffs[4 * i + 1] >> 6) | (a->coeffs[4 * i + 2] << 4));
        r[5 * i + 3] = (unsigned char) ((a->coeffs[4 * i + 2] >> 4) | (a->coeffs[4 * i + 3] << 6));
        r[5 * i + 4] = (unsigned char) (a->coeffs[4 * i + 3] >> 2);
    }
}

static void
poly_unpack_t1(poly *r, const unsigned char a[MLDSA44_POLYT1_PACKEDBYTES])
{
    unsigned int i;

    for (i = 0; i < MLDSA44_N / 4; i++) {
        r->coeffs[4 * i + 0] = ((a[5 * i + 0] >> 0) | ((uint32_t) a[5 * i + 1] << 8)) & 0x3FF;
        r->coeffs[4 * i + 1] = ((a[5 * i + 1] >> 2) | ((uint32_t) a[5 * i + 2] << 6)) & 0x3FF;
        r->coeffs[4 * i + 2] = ((a[5 * i + 2] >> 4) | ((uint32_t) a[5 * i + 3] << 4)) & 0x3FF;
        r->coeffs[4 * i + 3] = ((a[5 * i + 3] >> 6) | ((uint32_t) a[5 * i + 4] << 2)) & 0x3FF;
    }
}

static void
poly_pack_t0(unsigned char r[MLDSA44_POLYT0_PACKEDBYTES], const poly *a)
{
    unsigned int i, j;
    uint32_t     t[8];

    for (i = 0; i < MLDSA44_N / 8; i++) {
        for (j = 0; j < 8; j++) {
            t[j] = MLDSA44_Q + (1U << (MLDSA44_D - 1)) - a->coeffs[8 * i + j];
        }

        r[13 * i + 0]  = (unsigned char) (t[0] >> 0);
        r[13 * i + 1]  = (unsigned char) ((t[0] >> 8) | (t[1] << 5));
        r[13 * i + 2]  = (unsigned char) (t[1] >> 3);
        r[13 * i + 3]  = (unsigned char) ((t[1] >> 11) | (t[2] << 2));
        r[13 * i + 4]  = (unsigned char) ((t[2] >> 6) | (t[3] << 7));
        r[13 * i + 5]  = (unsigned char) (t[3] >> 1);
        r[13 * i + 6]  = (unsigned char) ((t[3] >> 9) | (t[4] << 4));
        r[13 * i + 7]  = (unsigned char) (t[4] >> 4);
        r[13 * i + 8]  = (unsigned char) ((t[4] >> 12) | (t[5] << 1));
        r[13 * i + 9]  = (unsigned char) ((t[5] >> 7) | (t[6] << 6));
        r[13 * i + 10] = (unsigned char) (t[6] >> 2);
        r[13 * i + 11] = (unsigned char) ((t[6] >> 10) | (t[7] << 3));
        r[13 * i + 12] = (unsigned char) (t[7] >> 5);
    }
}

static void
poly_unpack_t0(poly *r, const unsigned char a[MLDSA44_POLYT0_PACKEDBYTES])
{
    unsigned int i, j;
    uint32_t     t[8];

    for (i = 0; i < MLDSA44_N / 8; i++) {
        t[0] = (a[13 * i + 0] | ((uint32_t) a[13 * i + 1] << 8)) & 0x1FFF;
        t[1] = ((a[13 * i + 1] >> 5) | ((uint32_t) a[13 * i + 2] << 3) |
                ((uint32_t) a[13 * i + 3] << 11)) & 0x1FFF;
        t[2] = ((a[13 * i + 3] >> 2) | ((uint32_t) a[13 * i + 4] << 6)) & 0x1FFF;
        t[3] = ((a[13 * i + 4] >> 7) | ((uint32_t) a[13 * i + 5] << 1) |
                ((uint32_t) a[13 * i + 6] << 9)) & 0x1FFF;
        t[4] = ((a[13 * i + 6] >> 4) | ((uint32_t) a[13 * i + 7] << 4) |
                ((uint32_t) a[13 * i + 8] << 12)) & 0x1FFF;
        t[5] = ((a[13 * i + 8] >> 1) | ((uint32_t) a[13 * i + 9] << 7)) & 0x1FFF;
        t[6] = ((a[13 * i + 9] >> 6) | ((uint32_t) a[13 * i + 10] << 2) |
                ((uint32_t) a[13 * i + 11] << 10)) & 0x1FFF;
        t[7] = ((a[13 * i + 11] >> 3) | ((uint32_t) a[13 * i + 12] << 5)) & 0x1FFF;

        for (j = 0; j < 8; j++) {
            r->coeffs[8 * i + j] = MLDSA44_Q + (1U << (MLDSA44_D - 1)) - t[j];
        }
    }
}

static void
poly_pack_z(unsigned char r[MLDSA44_POLYZ_PACKEDBYTES], const poly *a)
{
    unsigned int i, j;
    uint32_t     t[4];

    for (i = 0; i < MLDSA44_N / 4; i++) {
        for (j = 0; j < 4; j++) {
            t[j] = MLDSA44_GAMMA1 - a->coeffs[4 * i + j];
            t[j] += (0U - (t[j] >> 31)) & MLDSA44_Q;
        }

        r[9 * i + 0] = (unsigned char) (t[0] >> 0);
        r[9 * i + 1] = (unsigned char) (t[0] >> 8);
        r[9 * i + 2] = (unsigned char) ((t[0] >> 16) | (t[1] << 2));
        r[9 * i + 3] = (unsigned char) (t[1] >> 6);
        r[9 * i + 4] = (unsigned char) ((t[1] >> 14) | (t[2] << 4));
        r[9 * i + 5] = (unsigned char) (t[2] >> 4);
        r[9 * i + 6] = (unsigned char) ((t[2] >> 12) | (t[3] << 6));
        r[9 * i + 7] = (unsigned char) (t[3] >> 2);
        r[9 * i + 8] = (unsigned char) (t[3] >> 10);
    }
}

static void
poly_unpack_z(poly *r, const unsigned char a[MLDSA44_POLYZ_PACKEDBYTES])
{
    unsigned int i, j;
    uint32_t     t[4];

    for (i = 0; i < MLDSA44_N / 4; i++) {
        t[0] = (a[9 * i + 0] | ((uint32_t) a[9 * i + 1] << 8) |
                ((uint32_t) a[9 * i + 2] << 16)) & 0x3FFFF;
        t[1] = ((a[9 * i + 2] >> 2) | ((uint32_t) a[9 * i + 3] << 6) |
                ((uint32_t) a[9 * i + 4] << 14)) & 0x3FFFF;
        t[2] = ((a[9 * i + 4] >> 4) | ((uint32_t) a[9 * i + 5] << 4) |
                ((uint32_t) a[9 * i + 6] << 12)) & 0x3FFFF;
        t[3] = ((a[9 * i + 6] >> 6) | ((uint32_t) a[9 * i + 7] << 2) |
                ((uint32_t) a[9 * i + 8] << 10)) & 0x3FFFF;

        for (j = 0; j < 4; j++) {
            t[j] = MLDSA44_GAMMA1 - t[j];
            t[j] += (0U - (t[j] >> 31)) & MLDSA44_Q;
            r->coeffs[4 * i + j] = t[j];
        }
    }
}

static void
poly_uniform_gamma1(poly *r, const unsigned char seed[MLDSA44_CRHBYTES], uint16_t nonce)
{
    unsigned char             buf[MLDSA44_POLYZ_PACKEDBYTES];
    crypto_xof_shake256_state state;
    unsigned char             extseed[MLDSA44_CRHBYTES + 2];

    memcpy(extseed, seed, MLDSA44_CRHBYTES);
    extseed[MLDSA44_CRHBYTES]     = (unsigned char) nonce;
    extseed[MLDSA44_CRHBYTES + 1] = (unsigned char) (nonce >> 8);

    crypto_xof_shake256_init(&state);
    crypto_xof_shake256_update(&state, extseed, sizeof extseed);
    crypto_xof_shake256_squeeze(&state, buf, sizeof buf);

    poly_unpack_z(r, buf);
    sodium_memzero(&state, sizeof state);
    sodium_memzero(buf, sizeof buf);
    sodium_memzero(extseed, sizeof extseed);
}

static void
poly_pack_w1(unsigned char r[MLDSA44_POLYW1_PACKEDBYTES], const poly *a)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_N / 4; i++) {
        r[3 * i + 0] = (unsigned char) (a->coeffs[4 * i + 0] | (a->coeffs[4 * i + 1] << 6));
        r[3 * i + 1] = (unsigned char) ((a->coeffs[4 * i + 1] >> 2) | (a->coeffs[4 * i + 2] << 4));
        r[3 * i + 2] = (unsigned char) ((a->coeffs[4 * i + 2] >> 4) | (a->coeffs[4 * i + 3] << 2));
    }
}

static void
polyvec_ntt(polyvec *r)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_ntt(&r->vec[i]);
    }
}

static void
polyvec_invntt(polyvec *r)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_invntt(&r->vec[i]);
    }
}

static void
polyvec_basemul_acc(poly *r, const polyvec *a, const polyvec *b)
{
    poly         t;
    unsigned int i;

    poly_basemul(r, &a->vec[0], &b->vec[0]);
    for (i = 1; i < MLDSA44_L; i++) {
        poly_basemul(&t, &a->vec[i], &b->vec[i]);
        poly_add(r, r, &t);
    }

    poly_reduce(r);
    sodium_memzero(&t, sizeof t);
}

static void
polyvec_basemul_poly(polyvec *r, const poly *a, const polyvec *b)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_basemul(&r->vec[i], a, &b->vec[i]);
    }
}

static void
polyvec_reduce(polyvec *r)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_reduce(&r->vec[i]);
    }
}

static void
polyvec_csubq(polyvec *r)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_csubq(&r->vec[i]);
    }
}

static void
polyvec_add(polyvec *r, const polyvec *a, const polyvec *b)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_add(&r->vec[i], &a->vec[i], &b->vec[i]);
    }
}

static void
polyvec_sub(polyvec *r, const polyvec *a, const polyvec *b)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_sub(&r->vec[i], &a->vec[i], &b->vec[i]);
    }
}

static void
polyvec_shiftl(polyvec *r)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_shiftl(&r->vec[i]);
    }
}

static uint32_t
polyvec_chknorm(const polyvec *a, uint32_t bound)
{
    unsigned int i;
    uint32_t     r = 0;

    for (i = 0; i < MLDSA44_K; i++) {
        r |= poly_chknorm(&a->vec[i], bound);
    }

    return r;
}

static void
polyvec_power2round(polyvec *a1, polyvec *a0, const polyvec *a)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_power2round(&a1->vec[i], &a0->vec[i], &a->vec[i]);
    }
}

static void
polyvec_decompose(polyvec *a1, polyvec *a0, const polyvec *a)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_decompose(&a1->vec[i], &a0->vec[i], &a->vec[i]);
    }
}

static unsigned int
polyvec_make_hint(polyvec *h, const polyvec *a0, const polyvec *a1)
{
    unsigned int i;
    unsigned int s = 0;

    for (i = 0; i < MLDSA44_K; i++) {
        s += poly_make_hint(&h->vec[i], &a0->vec[i], &a1->vec[i]);
    }

    return s;
}

static void
polyvec_use_hint(polyvec *r, const polyvec *a, const polyvec *h)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_use_hint(&r->vec[i], &a->vec[i], &h->vec[i]);
    }
}

static void
polyvec_uniform_gamma1(polyvec *r, const unsigned char seed[MLDSA44_CRHBYTES], uint16_t nonce)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_L; i++) {
        poly_uniform_gamma1(&r->vec[i], seed, (uint16_t) (nonce + i));
    }
}

static void
polyvec_pack_eta(unsigned char r[MLDSA44_K * MLDSA44_POLYETA_PACKEDBYTES], const polyvec *a)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_pack_eta(r + i * MLDSA44_POLYETA_PACKEDBYTES, &a->vec[i]);
    }
}

static uint32_t
polyvec_unpack_eta(polyvec *r, const unsigned char a[MLDSA44_K * MLDSA44_POLYETA_PACKEDBYTES])
{
    unsigned int i;
    uint32_t     bad = 0;

    for (i = 0; i < MLDSA44_K; i++) {
        bad |= poly_unpack_eta(&r->vec[i], a + i * MLDSA44_POLYETA_PACKEDBYTES);
    }

    return bad;
}

static void
polyvec_pack_t1(unsigned char r[MLDSA44_K * MLDSA44_POLYT1_PACKEDBYTES], const polyvec *a)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_pack_t1(r + i * MLDSA44_POLYT1_PACKEDBYTES, &a->vec[i]);
    }
}

static void
polyvec_unpack_t1(polyvec *r, const unsigned char a[MLDSA44_K * MLDSA44_POLYT1_PACKEDBYTES])
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_unpack_t1(&r->vec[i], a + i * MLDSA44_POLYT1_PACKEDBYTES);
    }
}

static void
polyvec_pack_t0(unsigned char r[MLDSA44_K * MLDSA44_POLYT0_PACKEDBYTES], const polyvec *a)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_pack_t0(r + i * MLDSA44_POLYT0_PACKEDBYTES, &a->vec[i]);
    }
}

static void
polyvec_unpack_t0(polyvec *r, const unsigned char a[MLDSA44_K * MLDSA44_POLYT0_PACKEDBYTES])
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_unpack_t0(&r->vec[i], a + i * MLDSA44_POLYT0_PACKEDBYTES);
    }
}

static void
polyvec_pack_z(unsigned char r[MLDSA44_L * MLDSA44_POLYZ_PACKEDBYTES], const polyvec *a)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_L; i++) {
        poly_pack_z(r + i * MLDSA44_POLYZ_PACKEDBYTES, &a->vec[i]);
    }
}

static void
polyvec_unpack_z(polyvec *r, const unsigned char a[MLDSA44_L * MLDSA44_POLYZ_PACKEDBYTES])
{
    unsigned int i;

    for (i = 0; i < MLDSA44_L; i++) {
        poly_unpack_z(&r->vec[i], a + i * MLDSA44_POLYZ_PACKEDBYTES);
    }
}

static void
polyvec_pack_w1(unsigned char r[MLDSA44_K * MLDSA44_POLYW1_PACKEDBYTES], const polyvec *a)
{
    unsigned int i;

    for (i = 0; i < MLDSA44_K; i++) {
        poly_pack_w1(r + i * MLDSA44_POLYW1_PACKEDBYTES, &a->vec[i]);
    }
}

static void
polyvec_pack_hint(unsigned char r[MLDSA44_POLYVECH_PACKEDBYTES], const polyvec *h)
{
    unsigned int i, j, k;

    memset(r, 0, MLDSA44_POLYVECH_PACKEDBYTES);
    k = 0;
    for (i = 0; i < MLDSA44_K; i++) {
        for (j = 0; j < MLDSA44_N; j++) {
            if (h->vec[i].coeffs[j] != 0) {
                r[k++] = (unsigned char) j;
            }
        }
        r[MLDSA44_OMEGA + i] = (unsigned char) k;
    }
}

static int
polyvec_unpack_hint(polyvec *h, const unsigned char a[MLDSA44_POLYVECH_PACKEDBYTES])
{
    unsigned int i, j, k;

    memset(h, 0, sizeof *h);
    k = 0;
    for (i = 0; i < MLDSA44_K; i++) {
        if (a[MLDSA44_OMEGA + i] < k || a[MLDSA44_OMEGA + i] > MLDSA44_OMEGA) {
            return -1;
        }
        for (j = k; j < a[MLDSA44_OMEGA + i]; j++) {
            if (j > k && a[j] <= a[j - 1]) {
                return -1;
            }
            h->vec[i].coeffs[a[j]] = 1;
        }
        k = a[MLDSA44_OMEGA + i];
    }
    if (sodium_is_zero(a + k, MLDSA44_OMEGA - k) == 0) {
        return -1;
    }

    return 0;
}

static unsigned int
rej_uniform(uint32_t *r, unsigned int len, const unsigned char *buf, unsigned int buflen)
{
    unsigned int ctr, pos;
    uint32_t     t;

    ctr = pos = 0;
    /* Variable-time rejection is fine here: callers only use public matrix seeds. */
    while (ctr < len && pos + 3 <= buflen) {
        t = (buf[pos + 0] | ((uint32_t) buf[pos + 1] << 8) | ((uint32_t) buf[pos + 2] << 16)) &
            0x7FFFFF;
        pos += 3;

        if (t < MLDSA44_Q) {
            r[ctr++] = t;
        }
    }

    return ctr;
}

#define GEN_MATRIX_NBLOCKS \
    ((3 * MLDSA44_N + crypto_xof_shake128_BLOCKBYTES - 1) / crypto_xof_shake128_BLOCKBYTES)

static void
gen_matrix(polyvec *a, const unsigned char seed[32])
{
    crypto_xof_shake128_state state;
    unsigned char             buf[GEN_MATRIX_NBLOCKS * crypto_xof_shake128_BLOCKBYTES];
    unsigned char             extseed[34];
    unsigned int              ctr, i, j;
    unsigned int              buflen;

    memcpy(extseed, seed, 32);

    for (i = 0; i < MLDSA44_K; i++) {
        for (j = 0; j < MLDSA44_L; j++) {
            extseed[32] = (unsigned char) j;
            extseed[33] = (unsigned char) i;

            crypto_xof_shake128_init(&state);
            crypto_xof_shake128_update(&state, extseed, 34);

            buflen = GEN_MATRIX_NBLOCKS * crypto_xof_shake128_BLOCKBYTES;
            crypto_xof_shake128_squeeze(&state, buf, buflen);

            ctr = rej_uniform(a[i].vec[j].coeffs, MLDSA44_N, buf, buflen);

            /* Refill count depends on public XOF output, not on secret key material. */
            while (ctr < MLDSA44_N) {
                crypto_xof_shake128_squeeze(&state, buf, crypto_xof_shake128_BLOCKBYTES);
                ctr += rej_uniform(a[i].vec[j].coeffs + ctr, MLDSA44_N - ctr, buf,
                                   crypto_xof_shake128_BLOCKBYTES);
            }
        }
    }
}

static void
hash_message(unsigned char       mu[MLDSA44_CRHBYTES],
             const unsigned char tr[MLDSA44_TRBYTES],
             const unsigned char *ctx,
             size_t               ctxlen,
             const unsigned char *m,
             unsigned long long   mlen)
{
    crypto_xof_shake256_state state;
    unsigned char             prefix[2];

    prefix[0] = 0;
    prefix[1] = (unsigned char) ctxlen;

    crypto_xof_shake256_init(&state);
    crypto_xof_shake256_update(&state, tr, MLDSA44_TRBYTES);
    crypto_xof_shake256_update(&state, prefix, sizeof prefix);
    crypto_xof_shake256_update(&state, ctx, ctxlen);
    crypto_xof_shake256_update(&state, m, mlen);
    crypto_xof_shake256_squeeze(&state, mu, MLDSA44_CRHBYTES);
    sodium_memzero(&state, sizeof state);
}

static void
hash_w1(unsigned char       ctilde[MLDSA44_CTILDEBYTES],
        const unsigned char mu[MLDSA44_CRHBYTES],
        const polyvec      *w1)
{
    crypto_xof_shake256_state state;
    unsigned char             w1packed[MLDSA44_K * MLDSA44_POLYW1_PACKEDBYTES];

    polyvec_pack_w1(w1packed, w1);
    crypto_xof_shake256_init(&state);
    crypto_xof_shake256_update(&state, mu, MLDSA44_CRHBYTES);
    crypto_xof_shake256_update(&state, w1packed, sizeof w1packed);
    crypto_xof_shake256_squeeze(&state, ctilde, MLDSA44_CTILDEBYTES);
    sodium_memzero(&state, sizeof state);
    sodium_memzero(w1packed, sizeof w1packed);
}

int
mldsa44_ref_seed_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *seed)
{
    polyvec                   mat[MLDSA44_K], s1, s2, t1, t0;
    crypto_xof_shake256_state state;
    unsigned char             buf[2 * MLDSA44_SEEDBYTES + MLDSA44_CRHBYTES];
    unsigned char             extseed[crypto_sign_mldsa44_SEEDBYTES + 2];
    unsigned char            *rho     = buf;
    unsigned char            *rhoprime = buf + MLDSA44_SEEDBYTES;
    unsigned char            *key      = buf + MLDSA44_SEEDBYTES + MLDSA44_CRHBYTES;
    unsigned int              i;

    COMPILER_ASSERT(MLDSA44_K == MLDSA44_L);
    COMPILER_ASSERT(crypto_sign_mldsa44_PUBLICKEYBYTES ==
                    MLDSA44_SEEDBYTES + MLDSA44_K * MLDSA44_POLYT1_PACKEDBYTES);
    COMPILER_ASSERT(crypto_sign_mldsa44_SECRETKEYBYTES ==
                    MLDSA44_SK_T0_OFFSET + MLDSA44_K * MLDSA44_POLYT0_PACKEDBYTES);
    COMPILER_ASSERT(crypto_sign_mldsa44_BYTES ==
                    MLDSA44_SIG_H_OFFSET + MLDSA44_POLYVECH_PACKEDBYTES);

    memcpy(extseed, seed, crypto_sign_mldsa44_SEEDBYTES);
    extseed[crypto_sign_mldsa44_SEEDBYTES]     = MLDSA44_K;
    extseed[crypto_sign_mldsa44_SEEDBYTES + 1] = MLDSA44_L;

    crypto_xof_shake256_init(&state);
    crypto_xof_shake256_update(&state, extseed, sizeof extseed);
    crypto_xof_shake256_squeeze(&state, buf, sizeof buf);

    gen_matrix(mat, rho);

    for (i = 0; i < MLDSA44_L; i++) {
        poly_getnoise_eta(&s1.vec[i], rhoprime, (uint16_t) i);
    }
    for (i = 0; i < MLDSA44_K; i++) {
        poly_getnoise_eta(&s2.vec[i], rhoprime, (uint16_t) (MLDSA44_L + i));
    }

    polyvec_pack_eta(sk + MLDSA44_SK_S1_OFFSET, &s1);
    polyvec_ntt(&s1);

    for (i = 0; i < MLDSA44_K; i++) {
        polyvec_basemul_acc(&t1.vec[i], &mat[i], &s1);
        poly_invntt(&t1.vec[i]);
    }

    polyvec_add(&t1, &t1, &s2);
    polyvec_reduce(&t1);
    polyvec_csubq(&t1);
    polyvec_power2round(&t1, &t0, &t1);

    memcpy(pk, rho, MLDSA44_SEEDBYTES);
    polyvec_pack_t1(pk + MLDSA44_SEEDBYTES, &t1);

    memcpy(sk, rho, MLDSA44_SEEDBYTES);
    memcpy(sk + MLDSA44_SK_KEY_OFFSET, key, MLDSA44_SEEDBYTES);
    crypto_xof_shake256(sk + MLDSA44_SK_TR_OFFSET, MLDSA44_TRBYTES, pk,
                        crypto_sign_mldsa44_PUBLICKEYBYTES);
    polyvec_pack_eta(sk + MLDSA44_SK_S2_OFFSET, &s2);
    polyvec_pack_t0(sk + MLDSA44_SK_T0_OFFSET, &t0);

    sodium_memzero(&state, sizeof state);
    sodium_memzero(buf, sizeof buf);
    sodium_memzero(extseed, sizeof extseed);
    sodium_memzero(&s1, sizeof s1);
    sodium_memzero(&s2, sizeof s2);
    sodium_memzero(&t0, sizeof t0);

    return 0;
}

int
mldsa44_ref_keypair(unsigned char *pk, unsigned char *sk)
{
    unsigned char seed[crypto_sign_mldsa44_SEEDBYTES];
    int           ret;

    randombytes_buf(seed, crypto_sign_mldsa44_SEEDBYTES);
    ret = mldsa44_ref_seed_keypair(pk, sk, seed);
    sodium_memzero(seed, sizeof seed);

    return ret;
}

int
mldsa44_ref_sign_detached(unsigned char *sig, unsigned long long *siglen_p,
                          const unsigned char *m, unsigned long long mlen,
                          const unsigned char *ctx, size_t ctxlen, const unsigned char *sk)
{
    polyvec                   mat[MLDSA44_K], s1, s2, t0, y, z, w0, w1, h;
    poly                      cp;
    crypto_xof_shake256_state state;
    unsigned char             buf[MLDSA44_SEEDBYTES + MLDSA44_RNDBYTES + MLDSA44_CRHBYTES];
    unsigned char             rhoprime[MLDSA44_CRHBYTES];
    unsigned char             ctilde[MLDSA44_CTILDEBYTES];
    unsigned char            *mu = buf + MLDSA44_SEEDBYTES + MLDSA44_RNDBYTES;
    unsigned int              n, i;
    uint32_t                  bad;
    int                       ret = -1;

    COMPILER_ASSERT(MLDSA44_SIGN_MAX_ITERATIONS * MLDSA44_L <= 65536);

    if (ctxlen > crypto_sign_mldsa44_CONTEXTBYTES_MAX) {
        goto end;
    }

    memcpy(buf, sk + MLDSA44_SK_KEY_OFFSET, MLDSA44_SEEDBYTES);
    randombytes_buf(buf + MLDSA44_SEEDBYTES, MLDSA44_RNDBYTES);
    bad = polyvec_unpack_eta(&s1, sk + MLDSA44_SK_S1_OFFSET);
    bad |= polyvec_unpack_eta(&s2, sk + MLDSA44_SK_S2_OFFSET);
    polyvec_unpack_t0(&t0, sk + MLDSA44_SK_T0_OFFSET);
    if (bad != 0) {
        goto end;
    }

    hash_message(mu, sk + MLDSA44_SK_TR_OFFSET, ctx, ctxlen, m, mlen);

    crypto_xof_shake256_init(&state);
    crypto_xof_shake256_update(&state, buf, sizeof buf);
    crypto_xof_shake256_squeeze(&state, rhoprime, sizeof rhoprime);

    gen_matrix(mat, sk);

    polyvec_ntt(&s1);
    polyvec_ntt(&s2);
    polyvec_ntt(&t0);

    for (n = 0; n < MLDSA44_SIGN_MAX_ITERATIONS; n++) {
        polyvec_uniform_gamma1(&y, rhoprime, (uint16_t) (n * MLDSA44_L));

        z = y;
        polyvec_ntt(&z);
        for (i = 0; i < MLDSA44_K; i++) {
            polyvec_basemul_acc(&w0.vec[i], &mat[i], &z);
            poly_invntt(&w0.vec[i]);
        }
        polyvec_csubq(&w0);
        polyvec_decompose(&w1, &w0, &w0);
        hash_w1(ctilde, mu, &w1);

        poly_challenge(&cp, ctilde);
        poly_ntt(&cp);

        polyvec_basemul_poly(&h, &cp, &s2);
        polyvec_invntt(&h);
        polyvec_sub(&w0, &w0, &h);
        polyvec_reduce(&w0);
        polyvec_csubq(&w0);
        if (polyvec_chknorm(&w0, MLDSA44_GAMMA2 - MLDSA44_BETA) != 0) {
            continue;
        }

        polyvec_basemul_poly(&z, &cp, &s1);
        polyvec_invntt(&z);
        polyvec_add(&z, &z, &y);
        polyvec_reduce(&z);
        polyvec_csubq(&z);
        if (polyvec_chknorm(&z, MLDSA44_GAMMA1 - MLDSA44_BETA) != 0) {
            continue;
        }

        polyvec_basemul_poly(&h, &cp, &t0);
        polyvec_invntt(&h);
        polyvec_csubq(&h);
        if (polyvec_chknorm(&h, MLDSA44_GAMMA2) != 0) {
            continue;
        }

        polyvec_add(&w0, &w0, &h);
        polyvec_csubq(&w0);
        if (polyvec_make_hint(&h, &w0, &w1) > MLDSA44_OMEGA) {
            continue;
        }

        memcpy(sig, ctilde, MLDSA44_CTILDEBYTES);
        polyvec_pack_z(sig + MLDSA44_SIG_Z_OFFSET, &z);
        polyvec_pack_hint(sig + MLDSA44_SIG_H_OFFSET, &h);
        ret = 0;
        break;
    }

end:
    sodium_memzero(&s1, sizeof s1);
    sodium_memzero(&s2, sizeof s2);
    sodium_memzero(&t0, sizeof t0);
    sodium_memzero(&y, sizeof y);
    sodium_memzero(&z, sizeof z);
    sodium_memzero(&w0, sizeof w0);
    sodium_memzero(&w1, sizeof w1);
    sodium_memzero(&h, sizeof h);
    sodium_memzero(&cp, sizeof cp);
    sodium_memzero(&state, sizeof state);
    sodium_memzero(buf, sizeof buf);
    sodium_memzero(rhoprime, sizeof rhoprime);
    sodium_memzero(ctilde, sizeof ctilde);
    if (ret != 0) {
        memset(sig, 0, crypto_sign_mldsa44_BYTES);
    }
    if (siglen_p != NULL) {
        *siglen_p = (ret == 0) ? crypto_sign_mldsa44_BYTES : 0U;
    }
    return ret;
}

int
mldsa44_ref_verify_detached(const unsigned char *sig, const unsigned char *m,
                            unsigned long long mlen, const unsigned char *ctx, size_t ctxlen,
                            const unsigned char *pk)
{
    polyvec                   mat[MLDSA44_K], z, t1, w1, h;
    poly                      cp;
    unsigned char             tr[MLDSA44_TRBYTES];
    unsigned char             mu[MLDSA44_CRHBYTES];
    unsigned char             ctilde[MLDSA44_CTILDEBYTES];
    unsigned int              i;
    int                       ret;

    if (ctxlen > crypto_sign_mldsa44_CONTEXTBYTES_MAX) {
        return -1;
    }

    polyvec_unpack_z(&z, sig + MLDSA44_SIG_Z_OFFSET);
    if (polyvec_chknorm(&z, MLDSA44_GAMMA1 - MLDSA44_BETA) != 0) {
        return -1;
    }
    if (polyvec_unpack_hint(&h, sig + MLDSA44_SIG_H_OFFSET) != 0) {
        return -1;
    }
    polyvec_unpack_t1(&t1, pk + MLDSA44_SEEDBYTES);

    crypto_xof_shake256(tr, MLDSA44_TRBYTES, pk, crypto_sign_mldsa44_PUBLICKEYBYTES);
    hash_message(mu, tr, ctx, ctxlen, m, mlen);

    gen_matrix(mat, pk);

    polyvec_ntt(&z);
    for (i = 0; i < MLDSA44_K; i++) {
        polyvec_basemul_acc(&w1.vec[i], &mat[i], &z);
    }

    poly_challenge(&cp, sig);
    poly_ntt(&cp);

    polyvec_shiftl(&t1);
    polyvec_ntt(&t1);
    polyvec_basemul_poly(&t1, &cp, &t1);

    polyvec_sub(&w1, &w1, &t1);
    polyvec_reduce(&w1);
    polyvec_invntt(&w1);
    polyvec_csubq(&w1);
    polyvec_use_hint(&w1, &w1, &h);
    hash_w1(ctilde, mu, &w1);

    ret = crypto_verify_32(ctilde, sig);

    sodium_memzero(mu, sizeof mu);

    return ret;
}
