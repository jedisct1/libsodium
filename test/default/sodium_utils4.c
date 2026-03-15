#define TEST_NAME "sodium_utils4"
#include "cmptest.h"

#undef sodium_malloc
#undef sodium_free
#undef sodium_allocarray

int
main(void)
{
    void *buf;

    buf = sodium_allocarray(10, 100);
    assert(buf != NULL);
    memset(buf, 0, 1000);
    sodium_free(buf);
    printf("allocarray: ok\n");

    buf = sodium_allocarray(0, 100);
    sodium_free(buf);
    printf("allocarray zero count: ok\n");

    buf = sodium_allocarray(100, 0);
    sodium_free(buf);
    printf("allocarray zero size: ok\n");

    buf = sodium_allocarray(SIZE_MAX / 2 + 1, SIZE_MAX / 2);
    assert(buf == NULL);
    printf("allocarray overflow: ok\n");

    buf = sodium_malloc(SIZE_MAX - 1U);
    assert(buf == NULL);
    printf("malloc SIZE_MAX-1: ok\n");

    sodium_free(NULL);
    printf("free NULL: ok\n");

    buf = sodium_malloc(100);
    assert(buf != NULL);
    memset(buf, 0x42, 100);
    sodium_mprotect_noaccess(buf);
    sodium_mprotect_readwrite(buf);
    assert(((unsigned char *) buf)[0] == 0x42);
    sodium_mprotect_readonly(buf);
    assert(((unsigned char *) buf)[0] == 0x42);
    sodium_mprotect_readwrite(buf);
    sodium_free(buf);
    printf("mprotect cycle: ok\n");

    return 0;
}
