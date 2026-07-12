
#define VEC4_QUARTERROUND(A, B, C, D)   \
    x_##A = vaddq_u32(x_##A, x_##B);    \
    t_##A = veorq_u32(x_##D, x_##A);    \
    x_##D = rotl32_16_neon(t_##A);      \
    x_##C = vaddq_u32(x_##C, x_##D);    \
    t_##C = veorq_u32(x_##B, x_##C);    \
    x_##B = rotl32_12_neon(t_##C);      \
    x_##A = vaddq_u32(x_##A, x_##B);    \
    t_##A = veorq_u32(x_##D, x_##A);    \
    x_##D = rotl32_8_neon(t_##A);       \
    x_##C = vaddq_u32(x_##C, x_##D);    \
    t_##C = veorq_u32(x_##B, x_##C);    \
    x_##B = rotl32_7_neon(t_##C)

if (bytes >= 256) {
    uint32x4_t x_0  = vdupq_n_u32(x[0]);
    uint32x4_t x_1  = vdupq_n_u32(x[1]);
    uint32x4_t x_2  = vdupq_n_u32(x[2]);
    uint32x4_t x_3  = vdupq_n_u32(x[3]);
    uint32x4_t x_4  = vdupq_n_u32(x[4]);
    uint32x4_t x_5  = vdupq_n_u32(x[5]);
    uint32x4_t x_6  = vdupq_n_u32(x[6]);
    uint32x4_t x_7  = vdupq_n_u32(x[7]);
    uint32x4_t x_8  = vdupq_n_u32(x[8]);
    uint32x4_t x_9  = vdupq_n_u32(x[9]);
    uint32x4_t x_10 = vdupq_n_u32(x[10]);
    uint32x4_t x_11 = vdupq_n_u32(x[11]);
    uint32x4_t x_12;
    uint32x4_t x_13;
    uint32x4_t x_14   = vdupq_n_u32(x[14]);
    uint32x4_t x_15   = vdupq_n_u32(x[15]);
    uint32x4_t orig0  = x_0;
    uint32x4_t orig1  = x_1;
    uint32x4_t orig2  = x_2;
    uint32x4_t orig3  = x_3;
    uint32x4_t orig4  = x_4;
    uint32x4_t orig5  = x_5;
    uint32x4_t orig6  = x_6;
    uint32x4_t orig7  = x_7;
    uint32x4_t orig8  = x_8;
    uint32x4_t orig9  = x_9;
    uint32x4_t orig10 = x_10;
    uint32x4_t orig11 = x_11;
    uint32x4_t orig12;
    uint32x4_t orig13;
    uint32x4_t orig14 = x_14;
    uint32x4_t orig15 = x_15;
    uint32x4_t t_0, t_1, t_2, t_3, t_4, t_5, t_6, t_7, t_8, t_9, t_10, t_11,
        t_12, t_13, t_14, t_15;

    uint32_t in12, in13;
    int      i;

    while (bytes >= 256) {
        static const uint64_t addv12_tbl[2] = { 0, 1 };
        static const uint64_t addv13_tbl[2] = { 2, 3 };
        uint64x2_t            t12, t13;
        uint64_t              in1213;

        x_0  = orig0;
        x_1  = orig1;
        x_2  = orig2;
        x_3  = orig3;
        x_4  = orig4;
        x_5  = orig5;
        x_6  = orig6;
        x_7  = orig7;
        x_8  = orig8;
        x_9  = orig9;
        x_10 = orig10;
        x_11 = orig11;
        x_14 = orig14;
        x_15 = orig15;

        in12   = x[12];
        in13   = x[13];
        in1213 = ((uint64_t) in12) | (((uint64_t) in13) << 32);

        t12 = vaddq_u64(vld1q_u64(addv12_tbl), vdupq_n_u64(in1213));
        t13 = vaddq_u64(vld1q_u64(addv13_tbl), vdupq_n_u64(in1213));

        x_12 = vuzp1q_u32(vreinterpretq_u32_u64(t12),
                          vreinterpretq_u32_u64(t13));
        x_13 = vuzp2q_u32(vreinterpretq_u32_u64(t12),
                          vreinterpretq_u32_u64(t13));

        orig12 = x_12;
        orig13 = x_13;

        in1213 += 4;

        x[12] = in1213 & 0xFFFFFFFF;
        x[13] = (in1213 >> 32) & 0xFFFFFFFF;

        for (i = 0; i < ROUNDS; i += 2) {
            VEC4_QUARTERROUND(0, 4, 8, 12);
            VEC4_QUARTERROUND(1, 5, 9, 13);
            VEC4_QUARTERROUND(2, 6, 10, 14);
            VEC4_QUARTERROUND(3, 7, 11, 15);
            VEC4_QUARTERROUND(0, 5, 10, 15);
            VEC4_QUARTERROUND(1, 6, 11, 12);
            VEC4_QUARTERROUND(2, 7, 8, 13);
            VEC4_QUARTERROUND(3, 4, 9, 14);
        }

#define ONEQUAD_TRANSPOSE(A, B, C, D)                                        \
    {                                                                        \
        uint32x4_t t0, t1, t2, t3;                                           \
                                                                             \
        x_##A = vaddq_u32(x_##A, orig##A);                                   \
        x_##B = vaddq_u32(x_##B, orig##B);                                   \
        x_##C = vaddq_u32(x_##C, orig##C);                                   \
        x_##D = vaddq_u32(x_##D, orig##D);                                   \
        t_##A = vtrn1q_u32(x_##A, x_##B);                                    \
        t_##B = vtrn2q_u32(x_##A, x_##B);                                    \
        t_##C = vtrn1q_u32(x_##C, x_##D);                                    \
        t_##D = vtrn2q_u32(x_##C, x_##D);                                    \
        x_##A = vreinterpretq_u32_u64(                                       \
            vtrn1q_u64(vreinterpretq_u64_u32(t_##A),                         \
                       vreinterpretq_u64_u32(t_##C)));                       \
        x_##B = vreinterpretq_u32_u64(                                       \
            vtrn1q_u64(vreinterpretq_u64_u32(t_##B),                         \
                       vreinterpretq_u64_u32(t_##D)));                       \
        x_##C = vreinterpretq_u32_u64(                                       \
            vtrn2q_u64(vreinterpretq_u64_u32(t_##A),                         \
                       vreinterpretq_u64_u32(t_##C)));                       \
        x_##D = vreinterpretq_u32_u64(                                       \
            vtrn2q_u64(vreinterpretq_u64_u32(t_##B),                         \
                       vreinterpretq_u64_u32(t_##D)));                       \
        t0 = veorq_u32(x_##A, vreinterpretq_u32_u8(vld1q_u8(m + 0)));        \
        vst1q_u8(c + 0, vreinterpretq_u8_u32(t0));                           \
        t1 = veorq_u32(x_##B, vreinterpretq_u32_u8(vld1q_u8(m + 64)));       \
        vst1q_u8(c + 64, vreinterpretq_u8_u32(t1));                          \
        t2 = veorq_u32(x_##C, vreinterpretq_u32_u8(vld1q_u8(m + 128)));      \
        vst1q_u8(c + 128, vreinterpretq_u8_u32(t2));                         \
        t3 = veorq_u32(x_##D, vreinterpretq_u32_u8(vld1q_u8(m + 192)));      \
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
