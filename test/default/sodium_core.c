
#define TEST_NAME "sodium_core"
#ifndef SGX
#include "cmptest.h"
#else
#include "test_enclave.h"
#endif

int misuse_status = 0;

static void
misuse_handler(void)
{
    printf("misuse_handler()\n");
#ifndef SGX
    exit(0);
#endif
}

int
main(void)
{
    sodium_set_misuse_handler(NULL);
    sodium_set_misuse_handler(misuse_handler);
    sodium_set_misuse_handler(NULL);

    assert(sodium_init() == 1);

    (void) sodium_runtime_has_neon();
    (void) sodium_runtime_has_sse2();
    (void) sodium_runtime_has_sse3();
    (void) sodium_runtime_has_ssse3();
    (void) sodium_runtime_has_sse41();
    (void) sodium_runtime_has_avx();
    (void) sodium_runtime_has_avx2();
    (void) sodium_runtime_has_avx512f();
    (void) sodium_runtime_has_pclmul();
    (void) sodium_runtime_has_aesni();
    (void) sodium_runtime_has_rdrand();

    sodium_set_misuse_handler(misuse_handler);
#if !defined(__EMSCRIPTEN__) && !defined(SGX)
    sodium_misuse();
    printf("Misuse handler returned\n");
#else
    printf("misuse_handler()\n");
#endif

    return 0;
}
