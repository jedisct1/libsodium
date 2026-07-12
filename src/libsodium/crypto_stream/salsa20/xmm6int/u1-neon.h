while (bytes >= 64) {
    uint32x4_t diag0 = vld1q_u32(x + 0);
    uint32x4_t diag1 = vld1q_u32(x + 4);
    uint32x4_t diag2 = vld1q_u32(x + 8);
    uint32x4_t diag3 = vld1q_u32(x + 12);
    uint32x4_t a;
    uint32x4x4_t rows;

    uint32_t in8;
    uint32_t in9;
    int      i;

    for (i = 0; i < ROUNDS; i += 2) {
        a     = vaddq_u32(diag0, diag1);
        diag3 = veorq_u32(diag3, rotl32_7_neon(a));
        a     = vaddq_u32(diag3, diag0);
        diag2 = veorq_u32(diag2, rotl32_9_neon(a));
        a     = vaddq_u32(diag2, diag3);
        diag1 = veorq_u32(diag1, rotl32_13_neon(a));
        a     = vaddq_u32(diag1, diag2);
        diag0 = veorq_u32(diag0, rotl32_18_neon(a));

        diag3 = vextq_u32(diag3, diag3, 3);
        diag2 = vextq_u32(diag2, diag2, 2);
        diag1 = vextq_u32(diag1, diag1, 1);

        a     = vaddq_u32(diag0, diag3);
        diag1 = veorq_u32(diag1, rotl32_7_neon(a));
        a     = vaddq_u32(diag1, diag0);
        diag2 = veorq_u32(diag2, rotl32_9_neon(a));
        a     = vaddq_u32(diag2, diag1);
        diag3 = veorq_u32(diag3, rotl32_13_neon(a));
        a     = vaddq_u32(diag3, diag2);
        diag0 = veorq_u32(diag0, rotl32_18_neon(a));

        diag1 = vextq_u32(diag1, diag1, 3);
        diag2 = vextq_u32(diag2, diag2, 2);
        diag3 = vextq_u32(diag3, diag3, 1);
    }

    diag0 = vaddq_u32(diag0, vld1q_u32(x + 0));
    diag1 = vaddq_u32(diag1, vld1q_u32(x + 4));
    diag2 = vaddq_u32(diag2, vld1q_u32(x + 8));
    diag3 = vaddq_u32(diag3, vld1q_u32(x + 12));

    rows = undiagonalize_neon(diag0, diag1, diag2, diag3);

    vst1q_u8(c + 0, vreinterpretq_u8_u32(veorq_u32(
        rows.val[0], vreinterpretq_u32_u8(vld1q_u8(m + 0)))));
    vst1q_u8(c + 16, vreinterpretq_u8_u32(veorq_u32(
        rows.val[1], vreinterpretq_u32_u8(vld1q_u8(m + 16)))));
    vst1q_u8(c + 32, vreinterpretq_u8_u32(veorq_u32(
        rows.val[2], vreinterpretq_u32_u8(vld1q_u8(m + 32)))));
    vst1q_u8(c + 48, vreinterpretq_u8_u32(veorq_u32(
        rows.val[3], vreinterpretq_u32_u8(vld1q_u8(m + 48)))));

    in8 = x[8];
    in9 = x[13];
    in8++;
    if (in8 == 0) {
        in9++;
    }
    x[8]  = in8;
    x[13] = in9;

    c += 64;
    m += 64;
    bytes -= 64;
}
