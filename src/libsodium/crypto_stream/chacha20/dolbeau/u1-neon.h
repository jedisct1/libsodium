while (bytes >= 64) {
    uint32x4_t x_0, x_1, x_2, x_3;

    uint32_t in12;
    uint32_t in13;
    int      i;

    x_0 = vld1q_u32(x + 0);
    x_1 = vld1q_u32(x + 4);
    x_2 = vld1q_u32(x + 8);
    x_3 = vld1q_u32(x + 12);

    for (i = 0; i < ROUNDS; i += 2) {
        x_0 = vaddq_u32(x_0, x_1);
        x_3 = veorq_u32(x_3, x_0);
        x_3 = rotl32_16_neon(x_3);

        x_2 = vaddq_u32(x_2, x_3);
        x_1 = veorq_u32(x_1, x_2);
        x_1 = rotl32_12_neon(x_1);

        x_0 = vaddq_u32(x_0, x_1);
        x_3 = veorq_u32(x_3, x_0);
        x_0 = vextq_u32(x_0, x_0, 3);
        x_3 = rotl32_8_neon(x_3);

        x_2 = vaddq_u32(x_2, x_3);
        x_3 = vextq_u32(x_3, x_3, 2);
        x_1 = veorq_u32(x_1, x_2);
        x_2 = vextq_u32(x_2, x_2, 1);
        x_1 = rotl32_7_neon(x_1);

        x_0 = vaddq_u32(x_0, x_1);
        x_3 = veorq_u32(x_3, x_0);
        x_3 = rotl32_16_neon(x_3);

        x_2 = vaddq_u32(x_2, x_3);
        x_1 = veorq_u32(x_1, x_2);
        x_1 = rotl32_12_neon(x_1);

        x_0 = vaddq_u32(x_0, x_1);
        x_3 = veorq_u32(x_3, x_0);
        x_0 = vextq_u32(x_0, x_0, 1);
        x_3 = rotl32_8_neon(x_3);

        x_2 = vaddq_u32(x_2, x_3);
        x_3 = vextq_u32(x_3, x_3, 2);
        x_1 = veorq_u32(x_1, x_2);
        x_2 = vextq_u32(x_2, x_2, 3);
        x_1 = rotl32_7_neon(x_1);
    }
    x_0 = vaddq_u32(x_0, vld1q_u32(x + 0));
    x_1 = vaddq_u32(x_1, vld1q_u32(x + 4));
    x_2 = vaddq_u32(x_2, vld1q_u32(x + 8));
    x_3 = vaddq_u32(x_3, vld1q_u32(x + 12));

    x_0 = veorq_u32(x_0, vreinterpretq_u32_u8(vld1q_u8(m + 0)));
    x_1 = veorq_u32(x_1, vreinterpretq_u32_u8(vld1q_u8(m + 16)));
    x_2 = veorq_u32(x_2, vreinterpretq_u32_u8(vld1q_u8(m + 32)));
    x_3 = veorq_u32(x_3, vreinterpretq_u32_u8(vld1q_u8(m + 48)));

    vst1q_u8(c + 0, vreinterpretq_u8_u32(x_0));
    vst1q_u8(c + 16, vreinterpretq_u8_u32(x_1));
    vst1q_u8(c + 32, vreinterpretq_u8_u32(x_2));
    vst1q_u8(c + 48, vreinterpretq_u8_u32(x_3));

    in12 = x[12];
    in13 = x[13];
    in12++;
    if (in12 == 0) {
        in13++;
    }
    x[12] = in12;
    x[13] = in13;

    bytes -= 64;
    c += 64;
    m += 64;
}
