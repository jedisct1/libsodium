
#define TEST_NAME "misuse"
#include "cmptest.h"

#include <signal.h>

static void
sigabrt_handler_6(int sig)
{
    (void) sig;
    exit(0);
}

static void
sigabrt_handler_5(int sig)
{
    (void) sig;
    signal(SIGABRT, sigabrt_handler_6);
    assert(crypto_aead_xchacha20poly1305_ietf_encrypt(NULL, NULL, NULL, UINT64_MAX,
                                                      NULL, 0, NULL, NULL, NULL) == -1);
    exit(1);
}

static void
sigabrt_handler_4(int sig)
{
    (void) sig;
    signal(SIGABRT, sigabrt_handler_5);
    assert(crypto_aead_chacha20poly1305_ietf_encrypt(NULL, NULL, NULL, UINT64_MAX,
                                                     NULL, 0, NULL, NULL, NULL) == -1);
    exit(1);
}

static void
sigabrt_handler_3(int sig)
{
    (void) sig;
    signal(SIGABRT, sigabrt_handler_4);
    assert(crypto_aead_chacha20poly1305_encrypt(NULL, NULL, NULL, UINT64_MAX,
                                                NULL, 0, NULL, NULL, NULL) == -1);
    exit(1);
}

static void
sigabrt_handler_2(int sig)
{
    (void) sig;
    signal(SIGABRT, sigabrt_handler_3);
#if SIZE_MAX > 0x4000000000ULL
    randombytes_buf_deterministic(NULL, 0x4000000001ULL, NULL);
#else
    abort();
#endif
    exit(1);
}

static void
sigabrt_handler_1(int sig)
{
    (void) sig;
    signal(SIGABRT, sigabrt_handler_2);
    assert(crypto_kx_server_session_keys(NULL, NULL, NULL, NULL, NULL) == -1);
    exit(1);
}

#if defined(SIGABRT) && (defined(__APPLE__) || defined(__OpenBSD__))
int
main(void)
{
    signal(SIGABRT, sigabrt_handler_1);
    assert(crypto_kx_client_session_keys(NULL, NULL, NULL, NULL, NULL) == -1);
    return 1;
}
#else
int
main(void)
{
    return 0;
}
#endif
