
#define VEC2_HALFROUND(T, D0, D1, D2, D3)      \
    T    = vaddq_u32(D0, D1);                  \
    D3   = veorq_u32(D3, rotl32_7_neon(T));    \
    T    = vaddq_u32(D3, D0);                  \
    D2   = veorq_u32(D2, rotl32_9_neon(T));    \
    T    = vaddq_u32(D2, D3);                  \
    D1   = veorq_u32(D1, rotl32_13_neon(T));   \
    T    = vaddq_u32(D1, D2);                  \
    D0   = veorq_u32(D0, rotl32_18_neon(T));   \
    D3   = vextq_u32(D3, D3, 3);               \
    D2   = vextq_u32(D2, D2, 2);               \
    D1   = vextq_u32(D1, D1, 1)

while (bytes >= 128) {
    uint32x4_t diag0 = vld1q_u32(x + 0);
    uint32x4_t diag1 = vld1q_u32(x + 4);
    uint32x4_t diag2 = vld1q_u32(x + 8);
    uint32x4_t diag3 = vld1q_u32(x + 12);
    uint32x4_t diag0b, diag1b, diag2b, diag3b;
    uint32x4_t orig2b, orig3b;
    uint32x4_t a, b;
    uint32x4x4_t rows;

    uint64_t in89;
    int      i;

    in89 = (((uint64_t) x[8]) | (((uint64_t) x[13]) << 32)) + 1;

    orig2b = vsetq_lane_u32((uint32_t) (in89 & 0xFFFFFFFF), diag2, 0);
    orig3b = vsetq_lane_u32((uint32_t) (in89 >> 32), diag3, 1);

    diag0b = diag0;
    diag1b = diag1;
    diag2b = orig2b;
    diag3b = orig3b;

    for (i = 0; i < ROUNDS; i += 2) {
        VEC2_HALFROUND(a, diag0, diag1, diag2, diag3);
        VEC2_HALFROUND(b, diag0b, diag1b, diag2b, diag3b);
        VEC2_HALFROUND(a, diag0, diag3, diag2, diag1);
        VEC2_HALFROUND(b, diag0b, diag3b, diag2b, diag1b);
    }

    diag0  = vaddq_u32(diag0, vld1q_u32(x + 0));
    diag1  = vaddq_u32(diag1, vld1q_u32(x + 4));
    diag2  = vaddq_u32(diag2, vld1q_u32(x + 8));
    diag3  = vaddq_u32(diag3, vld1q_u32(x + 12));
    diag0b = vaddq_u32(diag0b, vld1q_u32(x + 0));
    diag1b = vaddq_u32(diag1b, vld1q_u32(x + 4));
    diag2b = vaddq_u32(diag2b, orig2b);
    diag3b = vaddq_u32(diag3b, orig3b);

    rows = undiagonalize_neon(diag0, diag1, diag2, diag3);

    vst1q_u8(c + 0, vreinterpretq_u8_u32(veorq_u32(
        rows.val[0], vreinterpretq_u32_u8(vld1q_u8(m + 0)))));
    vst1q_u8(c + 16, vreinterpretq_u8_u32(veorq_u32(
        rows.val[1], vreinterpretq_u32_u8(vld1q_u8(m + 16)))));
    vst1q_u8(c + 32, vreinterpretq_u8_u32(veorq_u32(
        rows.val[2], vreinterpretq_u32_u8(vld1q_u8(m + 32)))));
    vst1q_u8(c + 48, vreinterpretq_u8_u32(veorq_u32(
        rows.val[3], vreinterpretq_u32_u8(vld1q_u8(m + 48)))));

    rows = undiagonalize_neon(diag0b, diag1b, diag2b, diag3b);

    vst1q_u8(c + 64, vreinterpretq_u8_u32(veorq_u32(
        rows.val[0], vreinterpretq_u32_u8(vld1q_u8(m + 64)))));
    vst1q_u8(c + 80, vreinterpretq_u8_u32(veorq_u32(
        rows.val[1], vreinterpretq_u32_u8(vld1q_u8(m + 80)))));
    vst1q_u8(c + 96, vreinterpretq_u8_u32(veorq_u32(
        rows.val[2], vreinterpretq_u32_u8(vld1q_u8(m + 96)))));
    vst1q_u8(c + 112, vreinterpretq_u8_u32(veorq_u32(
        rows.val[3], vreinterpretq_u32_u8(vld1q_u8(m + 112)))));

    in89 += 1;

    x[8]  = (uint32_t) (in89 & 0xFFFFFFFF);
    x[13] = (uint32_t) (in89 >> 32);

    c += 128;
    m += 128;
    bytes -= 128;
}
#undef VEC2_HALFROUND
