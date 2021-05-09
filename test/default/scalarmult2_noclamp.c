
#define TEST_NAME "scalarmult2_noclamp"
#include "cmptest.h"

static unsigned char bobsk[32] = { 0x35, 0x4a, 0x67, 0x27, 0x0b, 0x01, 0xff,
                                   0x9e, 0x25, 0x9c, 0x23, 0x50, 0x56, 0x95,
                                   0x9e, 0x00, 0x8b, 0x14, 0xbe, 0x2d, 0x05,
                                   0x60, 0xdd, 0x88, 0xd6, 0x6f, 0xc6, 0x2d,
                                   0x68, 0xf2, 0x7d, 0x35 };

static unsigned char whole_curve_generator[32] = { 0x06 };

int
main(void)
{
    unsigned char bobpk[32];
    char          hex[65];
    int           i;
    int           ret;

    ret = crypto_scalarmult_noclamp(bobpk, bobsk, whole_curve_generator);
    assert(ret == 0);
    sodium_bin2hex(hex, sizeof hex, bobpk, sizeof bobpk);
    printf("%s\n", hex);

    return 0;
}
