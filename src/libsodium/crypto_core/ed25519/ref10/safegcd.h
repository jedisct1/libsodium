/*
 * Constant-time inversion using Bernstein and Yang's safegcd algorithm.
 *
 * Starting at delta = 1/2, 590 steps suffice for moduli below 2^256.
 * Proof: https://github.com/sipa/safegcd-bounds/tree/master/coq
 *
 * A normalized value has all but its last limb in [0, 2^bits).
 * The last limb carries the sign.
 */

#ifdef HAVE_TI_MODE

# define SAFEGCD_LIMBS   5
# define SAFEGCD_BITS    62
# define SAFEGCD_BATCHES 10
# define SAFEGCD_LOAD    LOAD64_LE
# define SAFEGCD_STORE   STORE64_LE

typedef int64_t  safegcd_limb;
typedef uint64_t safegcd_ulimb;
typedef int128_t safegcd_wide;

#else

# define SAFEGCD_LIMBS   9
# define SAFEGCD_BITS    30
# define SAFEGCD_BATCHES 20
# define SAFEGCD_LOAD    LOAD32_LE
# define SAFEGCD_STORE   STORE32_LE

typedef int32_t  safegcd_limb;
typedef uint32_t safegcd_ulimb;
typedef int64_t  safegcd_wide;

#endif

#define SAFEGCD_MASK ((((safegcd_ulimb) 1) << SAFEGCD_BITS) - 1)

/*
 * The modulus must be odd and in (1, 2^256).
 * Limbs may be negative.
 * inv is the inverse of the modulus mod 2^bits.
 */
typedef struct {
    safegcd_limb  limbs[SAFEGCD_LIMBS];
    safegcd_ulimb inv;
} safegcd_modulus;

#ifndef HAVE_INLINE_ASM
static volatile safegcd_limb optblocker_limb;
#endif

/* Keeps the compiler from turning masks into branches. */
static inline safegcd_limb
safegcd_barrier(safegcd_limb mask)
{
#ifdef HAVE_INLINE_ASM
    __asm__ __volatile__("" : "+r"(mask));
#else
    mask ^= optblocker_limb;
#endif
    return mask;
}

static inline safegcd_limb
safegcd_sign_mask(const safegcd_limb x)
{
    return safegcd_barrier(x >> (SAFEGCD_BITS + 1));
}

static void
safegcd_carry(safegcd_limb x[SAFEGCD_LIMBS])
{
    int i;

    for (i = 0; i < SAFEGCD_LIMBS - 1; i++) {
        x[i + 1] += x[i] >> SAFEGCD_BITS;
        x[i] &= (safegcd_limb) SAFEGCD_MASK;
    }
}

/* x must be normalized. */
static void
safegcd_add_modulus_if_negative(safegcd_limb x[SAFEGCD_LIMBS],
                                const safegcd_modulus *m)
{
    const safegcd_limb mask = safegcd_sign_mask(x[SAFEGCD_LIMBS - 1]);
    int                i;

    for (i = 0; i < SAFEGCD_LIMBS; i++) {
        x[i] += m->limbs[i] & mask;
    }
    safegcd_carry(x);
}

/*
 * Correct the sign and reduce x to [0, modulus).
 * x must be normalized and in (-2 * modulus, modulus); sign is 0 or -1.
 */
static void
safegcd_normalize(safegcd_limb x[SAFEGCD_LIMBS], const safegcd_limb sign,
                  const safegcd_modulus *m)
{
    int i;

    safegcd_add_modulus_if_negative(x, m);
    for (i = 0; i < SAFEGCD_LIMBS; i++) {
        x[i] = (x[i] ^ sign) - sign;
    }
    safegcd_carry(x);
    safegcd_add_modulus_if_negative(x, m);
}

/*
 * Apply a batch to f and g, or to their coefficients d and e.
 * Divide by 2^bits, reducing d and e to (-2 * modulus, modulus).
 */
static void
safegcd_update(safegcd_limb x[SAFEGCD_LIMBS], safegcd_limb y[SAFEGCD_LIMBS],
               safegcd_limb t[2][2], const safegcd_modulus *m, const int reduce)
{
    const safegcd_limb sx = safegcd_sign_mask(x[SAFEGCD_LIMBS - 1]);
    const safegcd_limb sy = safegcd_sign_mask(y[SAFEGCD_LIMBS - 1]);
    safegcd_limb      *out[2];
    safegcd_wide       c[2] = { 0, 0 };
    safegcd_limb       md[2] = { 0, 0 };
    safegcd_ulimb      low;
    int                i, r;

    out[0] = x;
    out[1] = y;
    for (i = 0; i < SAFEGCD_LIMBS; i++) {
        for (r = 0; r < 2; r++) {
            c[r] += (safegcd_wide) t[r][0] * x[i] +
                    (safegcd_wide) t[r][1] * y[i];
            if (reduce) {
                if (i == 0) {
                    /* Make division exact while keeping the result in range. */
                    md[r] = (t[r][0] & sx) + (t[r][1] & sy);
                    low = m->inv * (safegcd_ulimb) c[r] +
                          (safegcd_ulimb) md[r];
                    md[r] -= (safegcd_limb) (low & SAFEGCD_MASK);
                }
                c[r] += (safegcd_wide) m->limbs[i] * md[r];
            }
            if (i > 0) {
                out[r][i - 1] =
                    (safegcd_limb) ((safegcd_ulimb) c[r] & SAFEGCD_MASK);
            }
            c[r] >>= SAFEGCD_BITS;
        }
    }
    for (r = 0; r < 2; r++) {
        out[r][SAFEGCD_LIMBS - 1] = (safegcd_limb) c[r];
    }
}

/*
 * Build a batch's matrix from the low bits of f and g.
 * Return the new zeta and store the matrix, scaled by 2^bits, in t.
 *
 * zeta = -(delta + 1/2).
 * Swap f and g when delta > 0 and g is odd.
 */
#ifdef HAVE_TI_MODE

static void
safegcd_unpack(int64_t r[2], const uint64_t x)
{
    const int64_t lo = (int64_t) (int32_t) (uint32_t) x;

    r[0] = lo;
    r[1] = (int64_t) (x - (uint64_t) lo) >> 32;
}

static int64_t
safegcd_divsteps(int64_t zeta, uint64_t f, uint64_t g, int64_t t[2][2])
{
    int64_t  h[2][2], p[2][2];
    uint64_t uv, qr, x, y;
    int64_t  c1, c2, c3;
    int      i, j, k, n;

    /* Scale 59 steps to match the 62-bit division. */
    t[0][0] = 8;
    t[0][1] = 0;
    t[1][0] = 0;
    t[1][1] = 8;

    /* Split at 30 steps so each matrix row fits in one word. */
    for (k = 0; k < 2; k++) {
        uv = 1;
        qr = (uint64_t) 1 << 32;
        for (n = 0; n < 30 - k; n++) {
            c1 = safegcd_sign_mask(zeta);
            c2 = safegcd_barrier(-(int64_t) (g & 1));
            c3 = c1 & c2;
            x = (f ^ (uint64_t) c1) - (uint64_t) c1;
            y = (uv ^ (uint64_t) c1) - (uint64_t) c1;
            f ^= (f ^ g) & (uint64_t) c3;
            uv ^= (uv ^ qr) & (uint64_t) c3;
            g = (g + (x & (uint64_t) c2)) >> 1;
            qr += y & (uint64_t) c2;
            zeta = (zeta ^ c3) - 1;
            uv <<= 1;
        }
        safegcd_unpack(h[0], uv);
        safegcd_unpack(h[1], qr);
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 2; j++) {
                p[i][j] = h[i][0] * t[0][j] + h[i][1] * t[1][j];
            }
        }
        memcpy(t, p, sizeof p);
    }
    return zeta;
}

#else

static int32_t
safegcd_divsteps(int32_t zeta, uint32_t f, uint32_t g, int32_t t[2][2])
{
    uint32_t u = 1, v = 0, q = 0, r = 1;
    uint32_t x, y, z;
    int32_t  c1, c2, c3;
    int      n;

    for (n = 0; n < 30; n++) {
        c1 = safegcd_sign_mask(zeta);
        c2 = safegcd_barrier(-(int32_t) (g & 1));
        c3 = c1 & c2;
        x = (f ^ (uint32_t) c1) - (uint32_t) c1;
        y = (u ^ (uint32_t) c1) - (uint32_t) c1;
        z = (v ^ (uint32_t) c1) - (uint32_t) c1;
        f ^= (f ^ g) & (uint32_t) c3;
        u ^= (u ^ q) & (uint32_t) c3;
        v ^= (v ^ r) & (uint32_t) c3;
        g = (g + (x & (uint32_t) c2)) >> 1;
        q += y & (uint32_t) c2;
        r += z & (uint32_t) c2;
        zeta = (zeta ^ c3) - 1;
        u <<= 1;
        v <<= 1;
    }
    t[0][0] = (int32_t) u;
    t[0][1] = (int32_t) v;
    t[1][0] = (int32_t) q;
    t[1][1] = (int32_t) r;

    return zeta;
}

#endif

/* Words have 2 more bits than limbs, hence the 2 * i shifts. */
static void
safegcd_from_bytes(safegcd_limb x[SAFEGCD_LIMBS], const unsigned char s[32])
{
    safegcd_ulimb w[SAFEGCD_LIMBS - 1];
    int           i;

    for (i = 0; i < SAFEGCD_LIMBS - 1; i++) {
        w[i] = SAFEGCD_LOAD(s + sizeof w[0] * i);
    }
    x[0] = (safegcd_limb) (w[0] & SAFEGCD_MASK);
    for (i = 1; i < SAFEGCD_LIMBS - 1; i++) {
        x[i] = (safegcd_limb) (((w[i - 1] >> (SAFEGCD_BITS + 2 - 2 * i)) |
                                (w[i] << (2 * i))) & SAFEGCD_MASK);
    }
    x[SAFEGCD_LIMBS - 1] = (safegcd_limb)
        (w[SAFEGCD_LIMBS - 2] >> (SAFEGCD_BITS + 2 - 2 * (SAFEGCD_LIMBS - 1)));
}

/* x must be normalized, and below 2^256. */
static void
safegcd_to_bytes(unsigned char s[32], const safegcd_limb x[SAFEGCD_LIMBS])
{
    int i;

    for (i = 0; i < SAFEGCD_LIMBS - 1; i++) {
        SAFEGCD_STORE(s + sizeof(safegcd_ulimb) * i,
                      ((safegcd_ulimb) x[i] >> (2 * i)) |
                      ((safegcd_ulimb) x[i + 1] << (SAFEGCD_BITS - 2 * i)));
    }
}

/*
 * Invert s, returning zero for zero.
 * s must be below the modulus; nonzero s must have an inverse.
 *
 * r and s can overlap.
 */
static void
safegcd_invert(unsigned char r[32], const unsigned char s[32],
               const safegcd_modulus *m)
{
    safegcd_limb f[SAFEGCD_LIMBS], g[SAFEGCD_LIMBS];
    safegcd_limb d[SAFEGCD_LIMBS], e[SAFEGCD_LIMBS];
    safegcd_limb t[2][2], t_prev[2][2];
    safegcd_limb zeta;
    int          i;

    memcpy(f, m->limbs, sizeof f);
    safegcd_from_bytes(g, s);
    memset(d, 0, sizeof d);
    memset(e, 0, sizeof e);
    e[0] = 1;

    /* Delay d and e so their updates can overlap the next batch. */
    zeta = safegcd_divsteps(-1, (safegcd_ulimb) f[0], (safegcd_ulimb) g[0], t);
    for (i = 1; i < SAFEGCD_BATCHES; i++) {
        safegcd_update(f, g, t, m, 0);
        memcpy(t_prev, t, sizeof t);
        zeta = safegcd_divsteps(zeta, (safegcd_ulimb) f[0],
                                (safegcd_ulimb) g[0], t);
        safegcd_update(d, e, t_prev, m, 1);
    }
    safegcd_update(f, g, t, m, 0);
    safegcd_update(d, e, t, m, 1);

    /* For nonzero s, f is 1 or -1 and gives the inverse's sign. */
    safegcd_normalize(d, safegcd_sign_mask(f[SAFEGCD_LIMBS - 1]), m);
    safegcd_to_bytes(r, d);
}
