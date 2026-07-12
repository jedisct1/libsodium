
#define VEC2_HALFROUND(P, E0, E2)          \
    P##_0 = vaddq_u32(P##_0, P##_1);       \
    P##_3 = veorq_u32(P##_3, P##_0);       \
    P##_3 = rotl32_16_neon(P##_3);         \
    P##_2 = vaddq_u32(P##_2, P##_3);       \
    P##_1 = veorq_u32(P##_1, P##_2);       \
    P##_1 = rotl32_12_neon(P##_1);         \
    P##_0 = vaddq_u32(P##_0, P##_1);       \
    P##_3 = veorq_u32(P##_3, P##_0);       \
    P##_0 = vextq_u32(P##_0, P##_0, E0);   \
    P##_3 = rotl32_8_neon(P##_3);          \
    P##_2 = vaddq_u32(P##_2, P##_3);       \
    P##_3 = vextq_u32(P##_3, P##_3, 2);    \
    P##_1 = veorq_u32(P##_1, P##_2);       \
    P##_2 = vextq_u32(P##_2, P##_2, E2);   \
    P##_1 = rotl32_7_neon(P##_1)

while (bytes >= 128) {
    static const uint64_t addv_tbl[2] = { 1, 0 };

    uint32x4_t orig0 = vld1q_u32(x + 0);
    uint32x4_t orig1 = vld1q_u32(x + 4);
    uint32x4_t orig2 = vld1q_u32(x + 8);
    uint32x4_t orig3 = vld1q_u32(x + 12);
    uint32x4_t orig3b = vreinterpretq_u32_u64(vaddq_u64(
        vreinterpretq_u64_u32(orig3), vld1q_u64(addv_tbl)));
    uint32x4_t x_0, x_1, x_2, x_3;
    uint32x4_t y_0, y_1, y_2, y_3;

    uint64_t in1213;
    int      i;

    x_0 = orig0;
    x_1 = orig1;
    x_2 = orig2;
    x_3 = orig3;
    y_0 = orig0;
    y_1 = orig1;
    y_2 = orig2;
    y_3 = orig3b;

    for (i = 0; i < ROUNDS; i += 2) {
        VEC2_HALFROUND(x, 3, 1);
        VEC2_HALFROUND(y, 3, 1);
        VEC2_HALFROUND(x, 1, 3);
        VEC2_HALFROUND(y, 1, 3);
    }
    x_0 = vaddq_u32(x_0, orig0);
    x_1 = vaddq_u32(x_1, orig1);
    x_2 = vaddq_u32(x_2, orig2);
    x_3 = vaddq_u32(x_3, orig3);
    y_0 = vaddq_u32(y_0, orig0);
    y_1 = vaddq_u32(y_1, orig1);
    y_2 = vaddq_u32(y_2, orig2);
    y_3 = vaddq_u32(y_3, orig3b);

    x_0 = veorq_u32(x_0, vreinterpretq_u32_u8(vld1q_u8(m + 0)));
    x_1 = veorq_u32(x_1, vreinterpretq_u32_u8(vld1q_u8(m + 16)));
    x_2 = veorq_u32(x_2, vreinterpretq_u32_u8(vld1q_u8(m + 32)));
    x_3 = veorq_u32(x_3, vreinterpretq_u32_u8(vld1q_u8(m + 48)));
    y_0 = veorq_u32(y_0, vreinterpretq_u32_u8(vld1q_u8(m + 64)));
    y_1 = veorq_u32(y_1, vreinterpretq_u32_u8(vld1q_u8(m + 80)));
    y_2 = veorq_u32(y_2, vreinterpretq_u32_u8(vld1q_u8(m + 96)));
    y_3 = veorq_u32(y_3, vreinterpretq_u32_u8(vld1q_u8(m + 112)));

    vst1q_u8(c + 0, vreinterpretq_u8_u32(x_0));
    vst1q_u8(c + 16, vreinterpretq_u8_u32(x_1));
    vst1q_u8(c + 32, vreinterpretq_u8_u32(x_2));
    vst1q_u8(c + 48, vreinterpretq_u8_u32(x_3));
    vst1q_u8(c + 64, vreinterpretq_u8_u32(y_0));
    vst1q_u8(c + 80, vreinterpretq_u8_u32(y_1));
    vst1q_u8(c + 96, vreinterpretq_u8_u32(y_2));
    vst1q_u8(c + 112, vreinterpretq_u8_u32(y_3));

    in1213 = ((uint64_t) x[12]) | (((uint64_t) x[13]) << 32);
    in1213 += 2;

    x[12] = (uint32_t) (in1213 & 0xFFFFFFFF);
    x[13] = (uint32_t) (in1213 >> 32);

    bytes -= 128;
    c += 128;
    m += 128;
}
#undef VEC2_HALFROUND
