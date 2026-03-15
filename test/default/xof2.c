
#define TEST_NAME "xof2"
#include "cmptest.h"

int
main(void)
{
    unsigned char out[64];
    unsigned char out2[64];
    int           ret;

    {
        crypto_xof_shake128_state state;

        crypto_xof_shake128_init(&state);
        crypto_xof_shake128_update(&state, (const unsigned char *) "test", 4);
        crypto_xof_shake128_squeeze(&state, out, 32);

        ret = crypto_xof_shake128_update(&state, (const unsigned char *) "x", 1);
        assert(ret == -1);
        crypto_xof_shake128_squeeze(&state, out2, 32);
        assert(memcmp(out, out2, 32) != 0);
    }
    printf("shake128 update-after-squeeze: ok\n");

    {
        crypto_xof_shake256_state state;

        crypto_xof_shake256_init(&state);
        crypto_xof_shake256_update(&state, (const unsigned char *) "test", 4);
        crypto_xof_shake256_squeeze(&state, out, 32);

        ret = crypto_xof_shake256_update(&state, (const unsigned char *) "x", 1);
        assert(ret == -1);
        crypto_xof_shake256_squeeze(&state, out2, 32);
        assert(memcmp(out, out2, 32) != 0);
    }
    printf("shake256 update-after-squeeze: ok\n");

    {
        crypto_xof_turboshake128_state state;

        crypto_xof_turboshake128_init(&state);
        crypto_xof_turboshake128_update(&state, (const unsigned char *) "test", 4);
        crypto_xof_turboshake128_squeeze(&state, out, 32);

        ret = crypto_xof_turboshake128_update(&state,
                                              (const unsigned char *) "x", 1);
        assert(ret == -1);
        crypto_xof_turboshake128_squeeze(&state, out2, 32);
        assert(memcmp(out, out2, 32) != 0);
    }
    printf("turboshake128 update-after-squeeze: ok\n");

    {
        crypto_xof_turboshake256_state state;

        crypto_xof_turboshake256_init(&state);
        crypto_xof_turboshake256_update(&state, (const unsigned char *) "test", 4);
        crypto_xof_turboshake256_squeeze(&state, out, 32);

        ret = crypto_xof_turboshake256_update(&state,
                                              (const unsigned char *) "x", 1);
        assert(ret == -1);
        crypto_xof_turboshake256_squeeze(&state, out2, 32);
        assert(memcmp(out, out2, 32) != 0);
    }
    printf("turboshake256 update-after-squeeze: ok\n");

    return 0;
}
