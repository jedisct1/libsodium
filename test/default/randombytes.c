
#define TEST_NAME "randombytes"
#include "cmptest.h"

static unsigned char x[65536];
static unsigned long long freq[256];

static int compat_tests(void)
{
    size_t i;

    memset(x, 0, sizeof x);
    randombytes(x, sizeof x);
    for (i = 0; i < 256; ++i) {
        freq[i] = 0;
    }
    for (i = 0; i < sizeof x; ++i) {
        ++freq[255 & (int)x[i]];
    }
    for (i = 0; i < 256; ++i) {
        if (!freq[i]) {
            printf("nacl_tests failed\n");
        }
    }
    return 0;
}

static int randombytes_tests(void)
{
    unsigned int f = 0U;
    unsigned int i;
    uint32_t     n;

#ifdef __EMSCRIPTEN__
    assert(strcmp(randombytes_implementation_name(), "sysrandom"));
#else
    assert(strcmp(randombytes_implementation_name(), "js"));
#endif
    randombytes(x, 1U);
    do {
        n = randombytes_random();
        f |= ((n >> 24) > 1);
        f |= ((n >> 16) > 1) << 1;
        f |= ((n >>  8) > 1) << 2;
        f |= ((n      ) > 1) << 3;
        f |= (n > 0x7fffffff) << 4;
    } while (f != 0x1f);
    randombytes_close();

    for (i = 0; i < 256; ++i) {
        freq[i] = 0;
    }
    for (i = 0; i < 65536; ++i) {
        ++freq[randombytes_uniform(256)];
    }
    for (i = 0; i < 256; ++i) {
        if (!freq[i]) {
            printf("randombytes_uniform() test failed\n");
        }
    }
    assert(randombytes_uniform(1U) == 0U);
    randombytes_close();
#ifndef __EMSCRIPTEN__
    randombytes_set_implementation(&randombytes_salsa20_implementation);
    assert(strcmp(randombytes_implementation_name(), "salsa20") == 0);
#endif
    randombytes_stir();
    for (i = 0; i < 256; ++i) {
        freq[i] = 0;
    }
    for (i = 0; i < 65536; ++i) {
        ++freq[randombytes_uniform(256)];
    }
    for (i = 0; i < 256; ++i) {
        if (!freq[i]) {
            printf("randombytes_uniform() test failed\n");
        }
    }
    memset(x, 0, sizeof x);
    randombytes_buf(x, sizeof x);
    for (i = 0; i < 256; ++i) {
        freq[i] = 0;
    }
    for (i = 0; i < sizeof x; ++i) {
        ++freq[255 & (int)x[i]];
    }
    for (i = 0; i < 256; ++i) {
        if (!freq[i]) {
            printf("randombytes_buf() test failed\n");
        }
    }
    assert(randombytes_uniform(1U) == 0U);
    randombytes_close();

    randombytes(x, 1U);
    randombytes_close();

    return 0;
}

static uint32_t randombytes_uniform_impl(const uint32_t upper_bound)
{
    return upper_bound;
}

static int impl_tests(void)
{
#ifndef __native_client__
    randombytes_implementation impl = randombytes_sysrandom_implementation;
#else
    randombytes_implementation impl = randombytes_nativeclient_implementation;
#endif
    uint32_t                   v = randombytes_random();

    impl.uniform = randombytes_uniform_impl;
    randombytes_close();
    randombytes_set_implementation(&impl);
    assert(randombytes_uniform(v) == v);
    assert(randombytes_uniform(v) == v);
    assert(randombytes_uniform(v) == v);
    assert(randombytes_uniform(v) == v);
    randombytes_close();
    impl.close = NULL;
    randombytes_close();

    return 0;
}

int main(void)
{
    compat_tests();
    randombytes_tests();
#ifndef __EMSCRIPTEN__
    impl_tests();
#endif
    printf("OK\n");

    return 0;
}
