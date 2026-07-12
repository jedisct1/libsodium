
#define VEC4_QUARTERROUND(A, B, C, D)               \
    do {                                            \
        uint32x4_t t;                               \
                                                    \
        t    = vaddq_u32(z##A, z##D);               \
        z##B = veorq_u32(z##B, rotl32_7_neon(t));   \
        t    = vaddq_u32(z##B, z##A);               \
        z##C = veorq_u32(z##C, rotl32_9_neon(t));   \
        t    = vaddq_u32(z##C, z##B);               \
        z##D = veorq_u32(z##D, rotl32_13_neon(t));  \
        t    = vaddq_u32(z##D, z##C);               \
        z##A = veorq_u32(z##A, rotl32_18_neon(t));  \
    } while (0)

if (bytes >= 256) {
    uint32x4_t y0, y1, y2, y3, y4, y5, y6, y7, y8, y9, y10, y11, y12, y13, y14,
        y15;
    uint32x4_t z0, z1, z2, z3, z4, z5, z6, z7, z8, z9, z10, z11, z12, z13, z14,
        z15;
    uint32x4_t orig0, orig1, orig2, orig3, orig4, orig5, orig6, orig7, orig8,
        orig9, orig10, orig11, orig12, orig13, orig14, orig15;

    uint32_t in8;
    uint32_t in9;
    int      i;

    z0  = vld1q_u32(x + 0);
    z5  = vdupq_laneq_u32(z0, 1);
    z10 = vdupq_laneq_u32(z0, 2);
    z15 = vdupq_laneq_u32(z0, 3);
    z0  = vdupq_laneq_u32(z0, 0);
    z1  = vld1q_u32(x + 4);
    z6  = vdupq_laneq_u32(z1, 2);
    z11 = vdupq_laneq_u32(z1, 3);
    z12 = vdupq_laneq_u32(z1, 0);
    z1  = vdupq_laneq_u32(z1, 1);
    z2  = vld1q_u32(x + 8);
    z7  = vdupq_laneq_u32(z2, 3);
    z13 = vdupq_laneq_u32(z2, 1);
    z2  = vdupq_laneq_u32(z2, 2);
    /* no z8 -> first half of the counter, will fill later */
    z3  = vld1q_u32(x + 12);
    z4  = vdupq_laneq_u32(z3, 0);
    z14 = vdupq_laneq_u32(z3, 2);
    z3  = vdupq_laneq_u32(z3, 3);
    /* no z9 -> second half of the counter, will fill later */
    orig0  = z0;
    orig1  = z1;
    orig2  = z2;
    orig3  = z3;
    orig4  = z4;
    orig5  = z5;
    orig6  = z6;
    orig7  = z7;
    orig10 = z10;
    orig11 = z11;
    orig12 = z12;
    orig13 = z13;
    orig14 = z14;
    orig15 = z15;

    while (bytes >= 256) {
        static const uint64_t addv8_tbl[2] = { 0, 1 };
        static const uint64_t addv9_tbl[2] = { 2, 3 };
        uint64x2_t            t8, t9;
        uint64_t              in89;

        in8  = x[8];
        in9  = x[13];
        in89 = ((uint64_t) in8) | (((uint64_t) in9) << 32);

        t8 = vaddq_u64(vld1q_u64(addv8_tbl), vdupq_n_u64(in89));
        t9 = vaddq_u64(vld1q_u64(addv9_tbl), vdupq_n_u64(in89));

        z8 = vuzp1q_u32(vreinterpretq_u32_u64(t8), vreinterpretq_u32_u64(t9));
        z9 = vuzp2q_u32(vreinterpretq_u32_u64(t8), vreinterpretq_u32_u64(t9));

        orig8 = z8;
        orig9 = z9;

        in89 += 4;

        x[8]  = in89 & 0xFFFFFFFF;
        x[13] = (in89 >> 32) & 0xFFFFFFFF;

        z0  = orig0;
        z1  = orig1;
        z2  = orig2;
        z3  = orig3;
        z4  = orig4;
        z5  = orig5;
        z6  = orig6;
        z7  = orig7;
        z10 = orig10;
        z11 = orig11;
        z12 = orig12;
        z13 = orig13;
        z14 = orig14;
        z15 = orig15;

        for (i = 0; i < ROUNDS; i += 2) {
            VEC4_QUARTERROUND(0, 4, 8, 12);
            VEC4_QUARTERROUND(5, 9, 13, 1);
            VEC4_QUARTERROUND(10, 14, 2, 6);
            VEC4_QUARTERROUND(15, 3, 7, 11);
            VEC4_QUARTERROUND(0, 1, 2, 3);
            VEC4_QUARTERROUND(5, 6, 7, 4);
            VEC4_QUARTERROUND(10, 11, 8, 9);
            VEC4_QUARTERROUND(15, 12, 13, 14);
        }

#define ONEQUAD_TRANSPOSE(A, B, C, D)                                        \
    {                                                                        \
        uint32x4_t t0, t1, t2, t3;                                           \
                                                                             \
        z##A = vaddq_u32(z##A, orig##A);                                     \
        z##B = vaddq_u32(z##B, orig##B);                                     \
        z##C = vaddq_u32(z##C, orig##C);                                     \
        z##D = vaddq_u32(z##D, orig##D);                                     \
        y##A = vtrn1q_u32(z##A, z##B);                                       \
        y##B = vtrn2q_u32(z##A, z##B);                                       \
        y##C = vtrn1q_u32(z##C, z##D);                                       \
        y##D = vtrn2q_u32(z##C, z##D);                                       \
        z##A = vreinterpretq_u32_u64(                                        \
            vtrn1q_u64(vreinterpretq_u64_u32(y##A),                          \
                       vreinterpretq_u64_u32(y##C)));                        \
        z##B = vreinterpretq_u32_u64(                                        \
            vtrn1q_u64(vreinterpretq_u64_u32(y##B),                          \
                       vreinterpretq_u64_u32(y##D)));                        \
        z##C = vreinterpretq_u32_u64(                                        \
            vtrn2q_u64(vreinterpretq_u64_u32(y##A),                          \
                       vreinterpretq_u64_u32(y##C)));                        \
        z##D = vreinterpretq_u32_u64(                                        \
            vtrn2q_u64(vreinterpretq_u64_u32(y##B),                          \
                       vreinterpretq_u64_u32(y##D)));                        \
        t0 = veorq_u32(z##A, vreinterpretq_u32_u8(vld1q_u8(m + 0)));         \
        vst1q_u8(c + 0, vreinterpretq_u8_u32(t0));                           \
        t1 = veorq_u32(z##B, vreinterpretq_u32_u8(vld1q_u8(m + 64)));        \
        vst1q_u8(c + 64, vreinterpretq_u8_u32(t1));                          \
        t2 = veorq_u32(z##C, vreinterpretq_u32_u8(vld1q_u8(m + 128)));       \
        vst1q_u8(c + 128, vreinterpretq_u8_u32(t2));                         \
        t3 = veorq_u32(z##D, vreinterpretq_u32_u8(vld1q_u8(m + 192)));       \
        vst1q_u8(c + 192, vreinterpretq_u8_u32(t3));                         \
    }

#define ONEQUAD(A, B, C, D) ONEQUAD_TRANSPOSE(A, B, C, D)

        ONEQUAD(0, 1, 2, 3);
        m += 16;
        c += 16;
        ONEQUAD(4, 5, 6, 7);
        m += 16;
        c += 16;
        ONEQUAD(8, 9, 10, 11);
        m += 16;
        c += 16;
        ONEQUAD(12, 13, 14, 15);
        m -= 48;
        c -= 48;

#undef ONEQUAD
#undef ONEQUAD_TRANSPOSE

        bytes -= 256;
        c += 256;
        m += 256;
    }
}
#undef VEC4_QUARTERROUND
