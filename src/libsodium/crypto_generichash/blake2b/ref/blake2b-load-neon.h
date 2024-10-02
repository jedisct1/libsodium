/*
   BLAKE2 reference source code package - reference C implementations

   Copyright 2012, Samuel Neves <sneves@dei.uc.pt>.  You may use this under the
   terms of the CC0, the OpenSSL Licence, or the Apache Public License 2.0, at
   your option.  The terms of these licenses can be found at:

   - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
   - OpenSSL license   : https://www.openssl.org/source/license.html
   - Apache 2.0        : http://www.apache.org/licenses/LICENSE-2.0

   More information about the BLAKE2 hash function can be found at
   https://blake2.net.
*/

#ifndef blake2b_load_neon_H
#define blake2b_load_neon_H

#define LOAD_MSG_0_1(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m0), vget_low_u64(m1));       \
        b1 = vcombine_u64(vget_low_u64(m2), vget_low_u64(m3));       \
    } while(0)

#define LOAD_MSG_0_2(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m0), vget_high_u64(m1));     \
        b1 = vcombine_u64(vget_high_u64(m2), vget_high_u64(m3));     \
    } while(0)

#define LOAD_MSG_0_3(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m4), vget_low_u64(m5));       \
        b1 = vcombine_u64(vget_low_u64(m6), vget_low_u64(m7));       \
    } while(0)

#define LOAD_MSG_0_4(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m4), vget_high_u64(m5));     \
        b1 = vcombine_u64(vget_high_u64(m6), vget_high_u64(m7));     \
    } while(0)

#define LOAD_MSG_1_1(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m7), vget_low_u64(m2));       \
        b1 = vcombine_u64(vget_high_u64(m4), vget_high_u64(m6));     \
    } while(0)

#define LOAD_MSG_1_2(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m5), vget_low_u64(m4));       \
        b1 = vextq_u64(m7, m3, 1);                                   \
    } while(0)

#define LOAD_MSG_1_3(b0, b1)                                         \
    do {                                                             \
        b0 = vextq_u64(m0, m0, 1);                                   \
        b1 = vcombine_u64(vget_high_u64(m5), vget_high_u64(m2));     \
    } while(0)

#define LOAD_MSG_1_4(b0, b1)                                         \
    do {                                                             \
      b0 = vcombine_u64(vget_low_u64(m6), vget_low_u64(m1));         \
      b1 = vcombine_u64(vget_high_u64(m3), vget_high_u64(m1));       \
    } while(0)

#define LOAD_MSG_2_1(b0, b1)                                         \
    do {                                                             \
        b0 = vextq_u64(m5, m6, 1);                                   \
        b1 = vcombine_u64(vget_high_u64(m2), vget_high_u64(m7));     \
    } while(0)

#define LOAD_MSG_2_2(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m4), vget_low_u64(m0));       \
        b1 = vcombine_u64(vget_low_u64(m1), vget_high_u64(m6));      \
    } while(0)

#define LOAD_MSG_2_3(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m5), vget_high_u64(m1));      \
        b1 = vcombine_u64(vget_high_u64(m3), vget_high_u64(m4));     \
    } while(0)

#define LOAD_MSG_2_4(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m7), vget_low_u64(m3));       \
        b1 = vextq_u64(m0, m2, 1);                                   \
    } while(0)

#define LOAD_MSG_3_1(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m3), vget_high_u64(m1));     \
        b1 = vcombine_u64(vget_high_u64(m6), vget_high_u64(m5));     \
    } while(0)

#define LOAD_MSG_3_2(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m4), vget_high_u64(m0));     \
        b1 = vcombine_u64(vget_low_u64(m6), vget_low_u64(m7));       \
    } while(0)

#define LOAD_MSG_3_3(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m1), vget_high_u64(m2));      \
        b1 = vcombine_u64(vget_low_u64(m2), vget_high_u64(m7));      \
    } while(0)

#define LOAD_MSG_3_4(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m3), vget_low_u64(m5));       \
        b1 = vcombine_u64(vget_low_u64(m0), vget_low_u64(m4));       \
    } while(0)

#define LOAD_MSG_4_1(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m4), vget_high_u64(m2));     \
        b1 = vcombine_u64(vget_low_u64(m1), vget_low_u64(m5));       \
    } while(0)

#define LOAD_MSG_4_2(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m0), vget_high_u64(m3));      \
        b1 = vcombine_u64(vget_low_u64(m2), vget_high_u64(m7));      \
    } while(0)

#define LOAD_MSG_4_3(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m7), vget_high_u64(m5));      \
        b1 = vcombine_u64(vget_low_u64(m3), vget_high_u64(m1));      \
    } while(0)

#define LOAD_MSG_4_4(b0, b1)                                         \
    do {                                                             \
        b0 = vextq_u64(m0, m6, 1);                                   \
        b1 = vcombine_u64(vget_low_u64(m4), vget_high_u64(m6));      \
    } while(0)

#define LOAD_MSG_5_1(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m1), vget_low_u64(m3));       \
        b1 = vcombine_u64(vget_low_u64(m0), vget_low_u64(m4));       \
    } while(0)

#define LOAD_MSG_5_2(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m6), vget_low_u64(m5));       \
        b1 = vcombine_u64(vget_high_u64(m5), vget_high_u64(m1));     \
    } while(0)

#define LOAD_MSG_5_3(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m2), vget_high_u64(m3));      \
        b1 = vcombine_u64(vget_high_u64(m7), vget_high_u64(m0));     \
    } while(0)

#define LOAD_MSG_5_4(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m6), vget_high_u64(m2));     \
        b1 = vcombine_u64(vget_low_u64(m7), vget_high_u64(m4));      \
    } while(0)

#define LOAD_MSG_6_1(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m6), vget_high_u64(m0));      \
        b1 = vcombine_u64(vget_low_u64(m7), vget_low_u64(m2));       \
    } while(0)

#define LOAD_MSG_6_2(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m2), vget_high_u64(m7));     \
        b1 = vextq_u64(m6, m5, 1);                                   \
    } while(0)

#define LOAD_MSG_6_3(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m0), vget_low_u64(m3));       \
        b1 = vextq_u64(m4, m4, 1);                                   \
    } while(0)

#define LOAD_MSG_6_4(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m3), vget_high_u64(m1));     \
        b1 = vcombine_u64(vget_low_u64(m1), vget_high_u64(m5));      \
    } while(0)

#define LOAD_MSG_7_1(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m6), vget_high_u64(m3));     \
        b1 = vcombine_u64(vget_low_u64(m6), vget_high_u64(m1));      \
    } while(0)

#define LOAD_MSG_7_2(b0, b1)                                         \
    do {                                                             \
        b0 = vextq_u64(m5, m7, 1);                                   \
        b1 = vcombine_u64(vget_high_u64(m0), vget_high_u64(m4));     \
    } while(0)

#define LOAD_MSG_7_3(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m2), vget_high_u64(m7));     \
        b1 = vcombine_u64(vget_low_u64(m4), vget_low_u64(m1));       \
    } while(0)

#define LOAD_MSG_7_4(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m0), vget_low_u64(m2));       \
        b1 = vcombine_u64(vget_low_u64(m3), vget_low_u64(m5));       \
    } while(0)

#define LOAD_MSG_8_1(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m3), vget_low_u64(m7));       \
        b1 = vextq_u64(m5, m0, 1);                                   \
    } while(0)

#define LOAD_MSG_8_2(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m7), vget_high_u64(m4));     \
        b1 = vextq_u64(m1, m4, 1);                                   \
    } while(0)

#define LOAD_MSG_8_3(b0, b1)                                         \
    do {                                                             \
        b0 = m6;                                                     \
        b1 = vextq_u64(m0, m5, 1);                                   \
    } while(0)

#define LOAD_MSG_8_4(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m1), vget_high_u64(m3));      \
        b1 = m2;                                                     \
    } while(0)

#define LOAD_MSG_9_1(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m5), vget_low_u64(m4));       \
        b1 = vcombine_u64(vget_high_u64(m3), vget_high_u64(m0));     \
    } while(0)

#define LOAD_MSG_9_2(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m1), vget_low_u64(m2));       \
        b1 = vcombine_u64(vget_low_u64(m3), vget_high_u64(m2));      \
    } while(0)

#define LOAD_MSG_9_3(b0, b1)                                         \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m7), vget_high_u64(m4));     \
        b1 = vcombine_u64(vget_high_u64(m1), vget_high_u64(m6));     \
    } while(0)

#define LOAD_MSG_9_4(b0, b1)                                         \
    do {                                                             \
        b0 = vextq_u64(m5, m7, 1);                                   \
        b1 = vcombine_u64(vget_low_u64(m6), vget_low_u64(m0));       \
    } while(0)

#define LOAD_MSG_10_1(b0, b1)                                        \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m0), vget_low_u64(m1));       \
        b1 = vcombine_u64(vget_low_u64(m2), vget_low_u64(m3));       \
    } while(0)

#define LOAD_MSG_10_2(b0, b1)                                        \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m0), vget_high_u64(m1));     \
        b1 = vcombine_u64(vget_high_u64(m2), vget_high_u64(m3));     \
    } while(0)

#define LOAD_MSG_10_3(b0, b1)                                        \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m4), vget_low_u64(m5));       \
        b1 = vcombine_u64(vget_low_u64(m6), vget_low_u64(m7));       \
    } while(0)

#define LOAD_MSG_10_4(b0, b1)                                        \
    do {                                                             \
        b0 = vcombine_u64(vget_high_u64(m4), vget_high_u64(m5));     \
        b1 = vcombine_u64(vget_high_u64(m6), vget_high_u64(m7));     \
    } while(0)

#define LOAD_MSG_11_1(b0, b1)                                        \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m7), vget_low_u64(m2));       \
        b1 = vcombine_u64(vget_high_u64(m4), vget_high_u64(m6));     \
    } while(0)

#define LOAD_MSG_11_2(b0, b1)                                        \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m5), vget_low_u64(m4));       \
        b1 = vextq_u64(m7, m3, 1);                                   \
    } while(0)

#define LOAD_MSG_11_3(b0, b1)                                        \
    do {                                                             \
        b0 = vextq_u64(m0, m0, 1);                                   \
        b1 = vcombine_u64(vget_high_u64(m5), vget_high_u64(m2));     \
    } while(0)

#define LOAD_MSG_11_4(b0, b1)                                        \
    do {                                                             \
        b0 = vcombine_u64(vget_low_u64(m6), vget_low_u64(m1));       \
        b1 = vcombine_u64(vget_high_u64(m3), vget_high_u64(m1));     \
    } while(0)

#endif
