if (bytes > 0) {
    uint32x4_t diag0 = vld1q_u32(x + 0);
    uint32x4_t diag1 = vld1q_u32(x + 4);
    uint32x4_t diag2 = vld1q_u32(x + 8);
    uint32x4_t diag3 = vld1q_u32(x + 12);
    uint32x4_t a;
    uint32x4x4_t rows;
    uint8_t    partialblock[64];

    unsigned int i;

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

    vst1q_u8(partialblock + 0, vreinterpretq_u8_u32(rows.val[0]));
    vst1q_u8(partialblock + 16, vreinterpretq_u8_u32(rows.val[1]));
    vst1q_u8(partialblock + 32, vreinterpretq_u8_u32(rows.val[2]));
    vst1q_u8(partialblock + 48, vreinterpretq_u8_u32(rows.val[3]));

    for (i = 0; i < bytes; i++) {
        c[i] = m[i] ^ partialblock[i];
    }

    sodium_memzero(partialblock, sizeof partialblock);
}
