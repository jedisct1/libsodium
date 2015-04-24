
#include <sys/types.h>

#include <limits.h>
#include <signal.h>

#define TEST_NAME "sodium_utils3"
#include "cmptest.h"

#ifdef __SANITIZE_ADDRESS__
# warning The sodium_utils3 test is expected to fail with address sanitizer
#endif

static void segv_handler(int sig)
{
    printf("Intentional segfault / bus error caught\n");
    printf("OK\n");
#ifdef SIGSEGV
    signal(SIGSEGV, SIG_DFL);
#endif
#ifdef SIGBUS
    signal(SIGBUS, SIG_DFL);
#endif
#ifdef SIGABRT
    signal(SIGABRT, SIG_DFL);
#endif
    exit(0);
}

int main(void)
{
    void *buf;
    size_t size;

#ifdef SIGSEGV
    signal(SIGSEGV, segv_handler);
#endif
#ifdef SIGBUS
    signal(SIGBUS, segv_handler);
#endif
#ifdef SIGABRT
    signal(SIGABRT, segv_handler);
#endif
    size = randombytes_uniform(100000U);
    buf = sodium_malloc(size);
    assert(buf != NULL);
    sodium_mprotect_noaccess(buf);
    sodium_mprotect_readwrite(buf);
#ifndef __EMSCRIPTEN__
    sodium_memzero(((unsigned char *)buf) - 8, 8U);
    sodium_mprotect_readonly(buf);
    sodium_free(buf);
    printf("Underflow not caught\n");
#endif
    return 0;
}
