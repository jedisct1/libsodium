#include <stdint.h>
#include <string.h>

#include "crypto_hash_sha3.h"
#include "crypto_kem_mlkem768.h"
#include "crypto_xof_shake128.h"
#include "crypto_xof_shake256.h"
#include "kem_mlkem768_ref.h"
#include "private/common.h"
#include "randombytes.h"
#include "utils.h"

#define MLKEM768_Q    3329
#define MLKEM768_N    256
#define MLKEM768_K    3
#define MLKEM768_ETA1 2
#define MLKEM768_ETA2 2
#define MLKEM768_DU   10
#define MLKEM768_DV   4

#define MLKEM768_POLYBYTES                 384
#define MLKEM768_POLYVECBYTES              (MLKEM768_K * MLKEM768_POLYBYTES)
#define MLKEM768_POLYCOMPRESSEDBYTES_DU    320
#define MLKEM768_POLYCOMPRESSEDBYTES_DV    128
#define MLKEM768_POLYVECCOMPRESSEDBYTES_DU (MLKEM768_K * MLKEM768_POLYCOMPRESSEDBYTES_DU)

typedef struct poly {
    int16_t coeffs[MLKEM768_N];
} poly;

typedef struct polyvec {
    poly vec[MLKEM768_K];
} polyvec;

static const int16_t zetas[128] = {
    2285, 2571, 2970, 1812, 1493, 1422, 287,  202,  3158, 622,  1577, 182,  962,  2127, 1855, 1468,
    573,  2004, 264,  383,  2500, 1458, 1727, 3199, 2648, 1017, 732,  608,  1787, 411,  3124, 1758,
    1223, 652,  2777, 1015, 2036, 1491, 3047, 1785, 516,  3321, 3009, 2663, 1711, 2167, 126,  1469,
    2476, 3239, 3058, 830,  107,  1908, 3082, 2378, 2931, 961,  1821, 2604, 448,  2264, 677,  2054,
    2226, 430,  555,  843,  2078, 871,  1550, 105,  422,  587,  177,  3094, 3038, 2869, 1574, 1653,
    3083, 778,  1159, 3182, 2552, 1483, 2727, 1119, 1739, 644,  2457, 349,  418,  329,  3173, 3254,
    817,  1097, 603,  610,  1322, 2044, 1864, 384,  2114, 3193, 1218, 1994, 2455, 220,  2142, 1670,
    2144, 1799, 2051, 794,  1819, 2475, 2459, 478,  3221, 3021, 996,  991,  958,  1869, 1522, 1628
};

static int16_t
montgomery_reduce(int32_t a)
{
    int16_t t;

    t = (int16_t) ((uint16_t) a * 62209U);
    t = (int16_t) ((a - (int32_t) t * MLKEM768_Q) >> 16);

    return t;
}

static int16_t
barrett_reduce(int16_t a)
{
    int16_t t;

    t = (int16_t) (((int32_t) a * 20159) >> 26);
    t = a - t * MLKEM768_Q;

    return t;
}

static int16_t
csubq(int16_t a)
{
    a -= MLKEM768_Q;
    a += (a >> 15) & MLKEM768_Q;

    return a;
}

static void
poly_ntt(poly *r)
{
    unsigned int len, start, j, k;
    int16_t      t, zeta;

    k = 1;
    for (len = 128; len >= 2; len >>= 1) {
        for (start = 0; start < MLKEM768_N; start = j + len) {
            zeta = zetas[k++];
            for (j = start; j < start + len; j++) {
                t                  = montgomery_reduce((int32_t) zeta * r->coeffs[j + len]);
                r->coeffs[j + len] = r->coeffs[j] - t;
                r->coeffs[j]       = r->coeffs[j] + t;
            }
        }
    }
}

static void
poly_invntt(poly *r)
{
    unsigned int  start, len, j, k;
    int16_t       t, zeta;
    const int16_t f = 1441;

    k = 127;
    for (len = 2; len <= 128; len <<= 1) {
        for (start = 0; start < MLKEM768_N; start = j + len) {
            zeta = zetas[k--];
            for (j = start; j < start + len; j++) {
                t                  = r->coeffs[j];
                r->coeffs[j]       = barrett_reduce(t + r->coeffs[j + len]);
                r->coeffs[j + len] = montgomery_reduce((int32_t) zeta * (r->coeffs[j + len] - t));
            }
        }
    }
    for (j = 0; j < MLKEM768_N; j++) {
        r->coeffs[j] = montgomery_reduce((int32_t) f * r->coeffs[j]);
    }
}

static void
poly_basemul(poly *r, const poly *a, const poly *b)
{
    unsigned int i;
    int16_t      zeta;

    for (i = 0; i < MLKEM768_N / 4; i++) {
        zeta = zetas[64 + i];

        r->coeffs[4 * i] = montgomery_reduce((int32_t) a->coeffs[4 * i + 1] * b->coeffs[4 * i + 1]);
        r->coeffs[4 * i] = montgomery_reduce((int32_t) r->coeffs[4 * i] * zeta);
        r->coeffs[4 * i] += montgomery_reduce((int32_t) a->coeffs[4 * i] * b->coeffs[4 * i]);

        r->coeffs[4 * i + 1] = montgomery_reduce((int32_t) a->coeffs[4 * i] * b->coeffs[4 * i + 1]);
        r->coeffs[4 * i + 1] +=
            montgomery_reduce((int32_t) a->coeffs[4 * i + 1] * b->coeffs[4 * i]);

        r->coeffs[4 * i + 2] =
            montgomery_reduce((int32_t) a->coeffs[4 * i + 3] * b->coeffs[4 * i + 3]);
        r->coeffs[4 * i + 2] = montgomery_reduce((int32_t) r->coeffs[4 * i + 2] * (-zeta));
        r->coeffs[4 * i + 2] +=
            montgomery_reduce((int32_t) a->coeffs[4 * i + 2] * b->coeffs[4 * i + 2]);

        r->coeffs[4 * i + 3] =
            montgomery_reduce((int32_t) a->coeffs[4 * i + 2] * b->coeffs[4 * i + 3]);
        r->coeffs[4 * i + 3] +=
            montgomery_reduce((int32_t) a->coeffs[4 * i + 3] * b->coeffs[4 * i + 2]);
    }
}

static void
poly_tomont(poly *r)
{
    unsigned int  i;
    const int16_t f = 1353;

    for (i = 0; i < MLKEM768_N; i++) {
        r->coeffs[i] = montgomery_reduce((int32_t) f * r->coeffs[i]);
    }
}

static void
poly_reduce(poly *r)
{
    unsigned int i;

    for (i = 0; i < MLKEM768_N; i++) {
        r->coeffs[i] = barrett_reduce(r->coeffs[i]);
    }
}

static void
poly_add(poly *r, const poly *a, const poly *b)
{
    unsigned int i;

    for (i = 0; i < MLKEM768_N; i++) {
        r->coeffs[i] = a->coeffs[i] + b->coeffs[i];
    }
}

static void
poly_sub(poly *r, const poly *a, const poly *b)
{
    unsigned int i;

    for (i = 0; i < MLKEM768_N; i++) {
        r->coeffs[i] = a->coeffs[i] - b->coeffs[i];
    }
}

static void
poly_csubq(poly *r)
{
    unsigned int i;

    for (i = 0; i < MLKEM768_N; i++) {
        r->coeffs[i] = csubq(r->coeffs[i]);
    }
}

static void
poly_cbd_eta2(poly *r, const unsigned char buf[128])
{
    unsigned int i, j;
    uint32_t     t, d;
    int16_t      a, b;

    for (i = 0; i < MLKEM768_N / 8; i++) {
        t = (uint32_t) buf[4 * i] | ((uint32_t) buf[4 * i + 1] << 8) |
            ((uint32_t) buf[4 * i + 2] << 16) | ((uint32_t) buf[4 * i + 3] << 24);

        d = t & 0x55555555;
        d += (t >> 1) & 0x55555555;

        for (j = 0; j < 8; j++) {
            a                    = (d >> (4 * j)) & 0x3;
            b                    = (d >> (4 * j + 2)) & 0x3;
            r->coeffs[8 * i + j] = a - b;
        }
    }
}

static void
poly_getnoise_eta2(poly *r, const unsigned char seed[32], uint8_t nonce)
{
    unsigned char             buf[MLKEM768_ETA2 * MLKEM768_N / 4];
    crypto_xof_shake256_state state;
    unsigned char             extseed[33];

    memcpy(extseed, seed, 32);
    extseed[32] = nonce;

    crypto_xof_shake256_init(&state);
    crypto_xof_shake256_update(&state, extseed, 33);
    crypto_xof_shake256_squeeze(&state, buf, sizeof(buf));

    poly_cbd_eta2(r, buf);
    sodium_memzero(&state, sizeof state);
    sodium_memzero(buf, sizeof buf);
}

static void
poly_frombytes(poly *r, const unsigned char a[MLKEM768_POLYBYTES])
{
    unsigned int i;

    for (i = 0; i < MLKEM768_N / 2; i++) {
        r->coeffs[2 * i]     = ((a[3 * i + 0] >> 0) | ((uint16_t) a[3 * i + 1] << 8)) & 0xFFF;
        r->coeffs[2 * i + 1] = ((a[3 * i + 1] >> 4) | ((uint16_t) a[3 * i + 2] << 4)) & 0xFFF;
    }
}

static void
poly_tobytes(unsigned char r[MLKEM768_POLYBYTES], const poly *a)
{
    unsigned int i;
    uint16_t     t0, t1;

    for (i = 0; i < MLKEM768_N / 2; i++) {
        t0           = a->coeffs[2 * i];
        t1           = a->coeffs[2 * i + 1];
        r[3 * i + 0] = (unsigned char) (t0 >> 0);
        r[3 * i + 1] = (unsigned char) ((t0 >> 8) | (t1 << 4));
        r[3 * i + 2] = (unsigned char) (t1 >> 4);
    }
}

static void
poly_frommsg(poly *r, const unsigned char msg[32])
{
    unsigned int i, j;
    int16_t      mask;

    for (i = 0; i < MLKEM768_N / 8; i++) {
        for (j = 0; j < 8; j++) {
            mask                 = -((msg[i] >> j) & 1);
            r->coeffs[8 * i + j] = mask & ((MLKEM768_Q + 1) / 2);
        }
    }
}

static void
poly_tomsg(unsigned char msg[32], const poly *a)
{
    unsigned int i, j;
    uint32_t     t;

    for (i = 0; i < MLKEM768_N / 8; i++) {
        msg[i] = 0;
        for (j = 0; j < 8; j++) {
            t = (uint32_t) a->coeffs[8 * i + j];
            t += (uint32_t) (((int32_t) a->coeffs[8 * i + j] >> 15) & MLKEM768_Q);
            t = (((t << 1) + MLKEM768_Q / 2) * 80635) >> 28;
            t &= 1;
            msg[i] |= t << j;
        }
    }
}

static void
poly_compress_du(unsigned char r[MLKEM768_POLYCOMPRESSEDBYTES_DU], const poly *a)
{
    uint32_t     t[4];
    unsigned int i, j;

    for (i = 0; i < MLKEM768_N / 4; i++) {
        for (j = 0; j < 4; j++) {
            t[j] = (uint32_t) a->coeffs[4 * i + j];
            t[j] += (uint32_t) (((int32_t) a->coeffs[4 * i + j] >> 15) & MLKEM768_Q);
            t[j] = (uint32_t) ((((uint64_t) t[j] << 10) + MLKEM768_Q / 2) * 161271ULL >> 29);
            t[j] &= 0x3ff;
        }

        r[5 * i + 0] = (unsigned char) (t[0] >> 0);
        r[5 * i + 1] = (unsigned char) ((t[0] >> 8) | (t[1] << 2));
        r[5 * i + 2] = (unsigned char) ((t[1] >> 6) | (t[2] << 4));
        r[5 * i + 3] = (unsigned char) ((t[2] >> 4) | (t[3] << 6));
        r[5 * i + 4] = (unsigned char) (t[3] >> 2);
    }
}

static void
poly_decompress_du(poly *r, const unsigned char a[MLKEM768_POLYCOMPRESSEDBYTES_DU])
{
    uint16_t     t[4];
    unsigned int i;

    for (i = 0; i < MLKEM768_N / 4; i++) {
        t[0] = (a[5 * i + 0] >> 0) | ((uint16_t) a[5 * i + 1] << 8);
        t[1] = (a[5 * i + 1] >> 2) | ((uint16_t) a[5 * i + 2] << 6);
        t[2] = (a[5 * i + 2] >> 4) | ((uint16_t) a[5 * i + 3] << 4);
        t[3] = (a[5 * i + 3] >> 6) | ((uint16_t) a[5 * i + 4] << 2);

        r->coeffs[4 * i + 0] = (int16_t) (((uint32_t) (t[0] & 0x3FF) * MLKEM768_Q + 512) >> 10);
        r->coeffs[4 * i + 1] = (int16_t) (((uint32_t) (t[1] & 0x3FF) * MLKEM768_Q + 512) >> 10);
        r->coeffs[4 * i + 2] = (int16_t) (((uint32_t) (t[2] & 0x3FF) * MLKEM768_Q + 512) >> 10);
        r->coeffs[4 * i + 3] = (int16_t) (((uint32_t) (t[3] & 0x3FF) * MLKEM768_Q + 512) >> 10);
    }
}

static void
poly_compress_dv(unsigned char r[MLKEM768_POLYCOMPRESSEDBYTES_DV], const poly *a)
{
    uint32_t     t[8];
    unsigned int i, j;

    for (i = 0; i < MLKEM768_N / 8; i++) {
        for (j = 0; j < 8; j++) {
            t[j] = (uint32_t) a->coeffs[8 * i + j];
            t[j] += (uint32_t) (((int32_t) a->coeffs[8 * i + j] >> 15) & MLKEM768_Q);
            t[j] = (uint32_t) ((((uint64_t) t[j] << 4) + MLKEM768_Q / 2) * 161271ULL >> 29);
            t[j] &= 0xf;
        }

        r[4 * i + 0] = (unsigned char) (t[0] | (t[1] << 4));
        r[4 * i + 1] = (unsigned char) (t[2] | (t[3] << 4));
        r[4 * i + 2] = (unsigned char) (t[4] | (t[5] << 4));
        r[4 * i + 3] = (unsigned char) (t[6] | (t[7] << 4));
    }
}

static void
poly_decompress_dv(poly *r, const unsigned char a[MLKEM768_POLYCOMPRESSEDBYTES_DV])
{
    unsigned int i;

    for (i = 0; i < MLKEM768_N / 2; i++) {
        r->coeffs[2 * i + 0] = (int16_t) ((((uint16_t) (a[i] & 15) * MLKEM768_Q) + 8) >> 4);
        r->coeffs[2 * i + 1] = (int16_t) ((((uint16_t) (a[i] >> 4) * MLKEM768_Q) + 8) >> 4);
    }
}

static void
polyvec_ntt(polyvec *r)
{
    unsigned int i;

    for (i = 0; i < MLKEM768_K; i++) {
        poly_ntt(&r->vec[i]);
    }
}

static void
polyvec_invntt(polyvec *r)
{
    unsigned int i;

    for (i = 0; i < MLKEM768_K; i++) {
        poly_invntt(&r->vec[i]);
    }
}

static void
polyvec_basemul_acc(poly *r, const polyvec *a, const polyvec *b)
{
    poly         t;
    unsigned int i;

    poly_basemul(r, &a->vec[0], &b->vec[0]);
    for (i = 1; i < MLKEM768_K; i++) {
        poly_basemul(&t, &a->vec[i], &b->vec[i]);
        poly_add(r, r, &t);
    }

    poly_reduce(r);
}

static void
polyvec_reduce(polyvec *r)
{
    unsigned int i;

    for (i = 0; i < MLKEM768_K; i++) {
        poly_reduce(&r->vec[i]);
    }
}

static void
polyvec_csubq(polyvec *r)
{
    unsigned int i;

    for (i = 0; i < MLKEM768_K; i++) {
        poly_csubq(&r->vec[i]);
    }
}

static void
polyvec_add(polyvec *r, const polyvec *a, const polyvec *b)
{
    unsigned int i;

    for (i = 0; i < MLKEM768_K; i++) {
        poly_add(&r->vec[i], &a->vec[i], &b->vec[i]);
    }
}

static void
polyvec_tobytes(unsigned char r[MLKEM768_POLYVECBYTES], const polyvec *a)
{
    unsigned int i;

    for (i = 0; i < MLKEM768_K; i++) {
        poly_tobytes(r + i * MLKEM768_POLYBYTES, &a->vec[i]);
    }
}

static void
polyvec_frombytes(polyvec *r, const unsigned char a[MLKEM768_POLYVECBYTES])
{
    unsigned int i;

    for (i = 0; i < MLKEM768_K; i++) {
        poly_frombytes(&r->vec[i], a + i * MLKEM768_POLYBYTES);
    }
}

static int
polyvec_is_canonical(const polyvec *a)
{
    unsigned int i, j;

    for (i = 0; i < MLKEM768_K; i++) {
        for (j = 0; j < MLKEM768_N; j++) {
            if ((uint16_t) a->vec[i].coeffs[j] >= MLKEM768_Q) {
                return 0;
            }
        }
    }
    return 1;
}

static void
polyvec_compress(unsigned char r[MLKEM768_POLYVECCOMPRESSEDBYTES_DU], const polyvec *a)
{
    unsigned int i;

    for (i = 0; i < MLKEM768_K; i++) {
        poly_compress_du(r + i * MLKEM768_POLYCOMPRESSEDBYTES_DU, &a->vec[i]);
    }
}

static void
polyvec_decompress(polyvec *r, const unsigned char a[MLKEM768_POLYVECCOMPRESSEDBYTES_DU])
{
    unsigned int i;

    for (i = 0; i < MLKEM768_K; i++) {
        poly_decompress_du(&r->vec[i], a + i * MLKEM768_POLYCOMPRESSEDBYTES_DU);
    }
}

static unsigned int
rej_uniform(int16_t *r, unsigned int len, const unsigned char *buf, unsigned int buflen)
{
    unsigned int ctr, pos;
    uint16_t     val0, val1;

    ctr = pos = 0;
    while (ctr < len && pos + 3 <= buflen) {
        val0 = ((buf[pos + 0] >> 0) | ((uint16_t) buf[pos + 1] << 8)) & 0xFFF;
        val1 = ((buf[pos + 1] >> 4) | ((uint16_t) buf[pos + 2] << 4)) & 0xFFF;
        pos += 3;

        if (val0 < MLKEM768_Q) {
            r[ctr++] = (int16_t) val0;
        }
        if (ctr < len && val1 < MLKEM768_Q) {
            r[ctr++] = (int16_t) val1;
        }
    }

    return ctr;
}

#define GEN_MATRIX_NBLOCKS                                                             \
    ((12 * MLKEM768_N / 8 * (1 << 12) / MLKEM768_Q + crypto_xof_shake128_BLOCKBYTES) / \
     crypto_xof_shake128_BLOCKBYTES)

static void
gen_matrix(polyvec *a, const unsigned char seed[32], int transposed)
{
    crypto_xof_shake128_state state;
    unsigned char             buf[GEN_MATRIX_NBLOCKS * crypto_xof_shake128_BLOCKBYTES + 2];
    unsigned char             extseed[34];
    unsigned int              ctr, i, j;
    unsigned int              buflen;

    memcpy(extseed, seed, 32);

    for (i = 0; i < MLKEM768_K; i++) {
        for (j = 0; j < MLKEM768_K; j++) {
            if (transposed) {
                extseed[32] = (unsigned char) i;
                extseed[33] = (unsigned char) j;
            } else {
                extseed[32] = (unsigned char) j;
                extseed[33] = (unsigned char) i;
            }

            crypto_xof_shake128_init(&state);
            crypto_xof_shake128_update(&state, extseed, 34);

            buflen = GEN_MATRIX_NBLOCKS * crypto_xof_shake128_BLOCKBYTES;
            crypto_xof_shake128_squeeze(&state, buf, buflen);

            ctr = rej_uniform(a[i].vec[j].coeffs, MLKEM768_N, buf, buflen);

            while (ctr < MLKEM768_N) {
                crypto_xof_shake128_squeeze(&state, buf, crypto_xof_shake128_BLOCKBYTES);
                ctr += rej_uniform(a[i].vec[j].coeffs + ctr, MLKEM768_N - ctr, buf,
                                   crypto_xof_shake128_BLOCKBYTES);
            }
        }
    }
}

static void
indcpa_keypair(unsigned char       pk[crypto_kem_mlkem768_PUBLICKEYBYTES],
               unsigned char       sk[MLKEM768_POLYVECBYTES],
               const unsigned char seed[32])
{
    polyvec        a[MLKEM768_K], e, pkpv, skpv;
    unsigned char  buf[64];
    unsigned char *publicseed = buf;
    unsigned char *noiseseed  = buf + 32;
    unsigned int   i;
    uint8_t        nonce = 0;

    crypto_hash_sha3512(buf, seed, 33);

    gen_matrix(a, publicseed, 0);

    for (i = 0; i < MLKEM768_K; i++) {
        poly_getnoise_eta2(&skpv.vec[i], noiseseed, nonce++);
    }
    for (i = 0; i < MLKEM768_K; i++) {
        poly_getnoise_eta2(&e.vec[i], noiseseed, nonce++);
    }

    polyvec_ntt(&skpv);
    polyvec_ntt(&e);

    for (i = 0; i < MLKEM768_K; i++) {
        polyvec_basemul_acc(&pkpv.vec[i], &a[i], &skpv);
        poly_tomont(&pkpv.vec[i]);
    }

    polyvec_add(&pkpv, &pkpv, &e);
    polyvec_reduce(&pkpv);
    polyvec_csubq(&pkpv);
    polyvec_reduce(&skpv);
    polyvec_csubq(&skpv);

    polyvec_tobytes(sk, &skpv);
    polyvec_tobytes(pk, &pkpv);
    memcpy(pk + MLKEM768_POLYVECBYTES, publicseed, 32);
    sodium_memzero(buf, sizeof buf);
    sodium_memzero(&skpv, sizeof skpv);
    sodium_memzero(&e, sizeof e);
}

static void
indcpa_enc(unsigned char       ct[crypto_kem_mlkem768_CIPHERTEXTBYTES],
           const unsigned char m[32],
           const unsigned char pk[crypto_kem_mlkem768_PUBLICKEYBYTES],
           const unsigned char coins[32])
{
    polyvec       sp, pkpv, ep, at[MLKEM768_K], b;
    poly          v, k, epp;
    unsigned char seed[32];
    unsigned int  i;
    uint8_t       nonce = 0;

    memcpy(seed, pk + MLKEM768_POLYVECBYTES, 32);

    polyvec_frombytes(&pkpv, pk);

    poly_frommsg(&k, m);

    gen_matrix(at, seed, 1);

    for (i = 0; i < MLKEM768_K; i++) {
        poly_getnoise_eta2(&sp.vec[i], coins, nonce++);
    }
    for (i = 0; i < MLKEM768_K; i++) {
        poly_getnoise_eta2(&ep.vec[i], coins, nonce++);
    }
    poly_getnoise_eta2(&epp, coins, nonce++);

    polyvec_ntt(&sp);
    polyvec_reduce(&sp);

    for (i = 0; i < MLKEM768_K; i++) {
        polyvec_basemul_acc(&b.vec[i], &at[i], &sp);
    }

    polyvec_basemul_acc(&v, &pkpv, &sp);

    polyvec_invntt(&b);
    poly_invntt(&v);

    polyvec_add(&b, &b, &ep);
    poly_add(&v, &v, &epp);
    poly_add(&v, &v, &k);

    polyvec_reduce(&b);
    poly_reduce(&v);
    polyvec_csubq(&b);
    poly_csubq(&v);

    polyvec_compress(ct, &b);
    poly_compress_dv(ct + MLKEM768_POLYVECCOMPRESSEDBYTES_DU, &v);
    sodium_memzero(&sp, sizeof sp);
    sodium_memzero(&ep, sizeof ep);
    sodium_memzero(&epp, sizeof epp);
    sodium_memzero(&k, sizeof k);
}

static void
indcpa_dec(unsigned char       m[32],
           const unsigned char ct[crypto_kem_mlkem768_CIPHERTEXTBYTES],
           const unsigned char sk[MLKEM768_POLYVECBYTES])
{
    polyvec b, skpv;
    poly    v, mp;

    polyvec_decompress(&b, ct);
    poly_decompress_dv(&v, ct + MLKEM768_POLYVECCOMPRESSEDBYTES_DU);

    polyvec_frombytes(&skpv, sk);

    polyvec_ntt(&b);
    polyvec_reduce(&b);
    polyvec_basemul_acc(&mp, &skpv, &b);
    poly_invntt(&mp);

    poly_sub(&mp, &v, &mp);
    poly_reduce(&mp);
    poly_csubq(&mp);

    poly_tomsg(m, &mp);
    sodium_memzero(&skpv, sizeof skpv);
    sodium_memzero(&mp, sizeof mp);
}

static void
cmov(unsigned char *r, const unsigned char *x, size_t len, unsigned char b)
{
    size_t        i;
    unsigned char mask;

    mask = (unsigned char) (-(int) b);

#ifdef HAVE_INLINE_ASM
    __asm__ __volatile__("" : "+r"(mask));
#endif

    for (i = 0; i < len; i++) {
        r[i] ^= mask & (r[i] ^ x[i]);
    }
}

int
mlkem768_ref_seed_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *seed)
{
    unsigned char indseed[33];

    memcpy(indseed, seed, 32);
    indseed[32] = MLKEM768_K;

    indcpa_keypair(pk, sk, indseed);
    sodium_memzero(indseed, sizeof indseed);
    memcpy(sk + MLKEM768_POLYVECBYTES, pk, crypto_kem_mlkem768_PUBLICKEYBYTES);
    crypto_hash_sha3256(sk + MLKEM768_POLYVECBYTES + crypto_kem_mlkem768_PUBLICKEYBYTES, pk,
                        crypto_kem_mlkem768_PUBLICKEYBYTES);
    memcpy(sk + MLKEM768_POLYVECBYTES + crypto_kem_mlkem768_PUBLICKEYBYTES + 32, seed + 32, 32);

    return 0;
}

int
mlkem768_ref_keypair(unsigned char *pk, unsigned char *sk)
{
    unsigned char seed[crypto_kem_mlkem768_SEEDBYTES];

    randombytes_buf(seed, crypto_kem_mlkem768_SEEDBYTES);
    return mlkem768_ref_seed_keypair(pk, sk, seed);
}

int
mlkem768_ref_enc_deterministic(unsigned char *ct, unsigned char *ss, const unsigned char *pk,
                               const unsigned char *seed)
{
    polyvec       pkpv;
    unsigned char buf[64];
    unsigned char kr[64];

    polyvec_frombytes(&pkpv, pk);
    if (polyvec_is_canonical(&pkpv) == 0) {
        return -1;
    }

    memcpy(buf, seed, 32);
    crypto_hash_sha3256(buf + 32, pk, crypto_kem_mlkem768_PUBLICKEYBYTES);

    crypto_hash_sha3512(kr, buf, 64);

    indcpa_enc(ct, buf, pk, kr + 32);

    memcpy(ss, kr, crypto_kem_mlkem768_SHAREDSECRETBYTES);
    sodium_memzero(buf, sizeof buf);
    sodium_memzero(kr, sizeof kr);

    return 0;
}

int
mlkem768_ref_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk)
{
    unsigned char seed[32];

    randombytes_buf(seed, 32);
    return mlkem768_ref_enc_deterministic(ct, ss, pk, seed);
}

int
mlkem768_ref_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk)
{
    unsigned char        buf[64];
    unsigned char        kr[64];
    unsigned char        k_bar[crypto_kem_mlkem768_SHAREDSECRETBYTES];
    unsigned char        cmp[crypto_kem_mlkem768_CIPHERTEXTBYTES];
    const unsigned char *pk  = sk + MLKEM768_POLYVECBYTES;
    const unsigned char *hpk = sk + MLKEM768_POLYVECBYTES + crypto_kem_mlkem768_PUBLICKEYBYTES;
    const unsigned char *z   = sk + MLKEM768_POLYVECBYTES + crypto_kem_mlkem768_PUBLICKEYBYTES + 32;
    int                  fail;
    unsigned int         fail_mask;
    crypto_xof_shake256_state state;

    indcpa_dec(buf, ct, sk);

    memcpy(buf + 32, hpk, 32);

    crypto_hash_sha3512(kr, buf, 64);

    indcpa_enc(cmp, buf, pk, kr + 32);

    fail = sodium_memcmp(ct, cmp, crypto_kem_mlkem768_CIPHERTEXTBYTES);
    fail_mask = (unsigned int) fail;
    fail_mask >>= (sizeof(fail_mask) * 8U - 1U);

    crypto_xof_shake256_init(&state);
    crypto_xof_shake256_update(&state, z, 32);
    crypto_xof_shake256_update(&state, ct, crypto_kem_mlkem768_CIPHERTEXTBYTES);
    crypto_xof_shake256_squeeze(&state, k_bar, crypto_kem_mlkem768_SHAREDSECRETBYTES);

    cmov(kr, k_bar, crypto_kem_mlkem768_SHAREDSECRETBYTES, (unsigned char) fail_mask);

    memcpy(ss, kr, crypto_kem_mlkem768_SHAREDSECRETBYTES);
    sodium_memzero(buf, sizeof buf);
    sodium_memzero(kr, sizeof kr);
    sodium_memzero(k_bar, sizeof k_bar);
    sodium_memzero(cmp, sizeof cmp);
    sodium_memzero(&state, sizeof state);

    return 0;
}
