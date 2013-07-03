
#include <sys/types.h>
#include <sys/time.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "crypto_core_salsa20.h"
#include "crypto_auth_hmacsha512256.h"
#include "crypto_stream_salsa20.h"
#include "randombytes.h"
#include "randombytes_salsa20_random.h"
#include "utils.h"

#ifdef _WIN32
# include <windows.h>
# include <wincrypt.h>
# include <sys/timeb.h>
#endif

#define SALSA20_RANDOM_BLOCK_SIZE crypto_core_salsa20_OUTPUTBYTES
#define SHA512_BLOCK_SIZE 128U
#define SHA512_MIN_PAD_SIZE (1U + 16U)
#define COMPILER_ASSERT(X) (void) sizeof(char[(X) ? 1 : -1])

typedef struct Salsa20Random_ {
    unsigned char key[crypto_stream_salsa20_KEYBYTES];
    unsigned char rnd32[SALSA20_RANDOM_BLOCK_SIZE];
    uint64_t      nonce;
    size_t        rnd32_outleft;
    pid_t         pid;
#ifdef _WIN32
    HCRYPTPROV    hcrypt_prov;
#endif
    int           random_data_source_fd;
    bool          initialized;
} Salsa20Random;

static Salsa20Random stream = {
    _SODIUM_C99(.random_data_source_fd =) -1,
    _SODIUM_C99(.rnd32_outleft =) (size_t) 0U,
    _SODIUM_C99(.initialized =) 0
};

static uint64_t
sodium_hrtime(void)
{
    struct timeval tv;
    uint64_t       ts = (uint64_t) 0U;
    int            ret;

#ifdef _WIN32
    struct _timeb tb;

    _ftime(&tb);
    tv.tv_sec = (long) tb.time;
    tv.tv_usec = ((int) tb.millitm) * 1000;
    ret = 0;
#else
    ret = gettimeofday(&tv, NULL);
#endif
    assert(ret == 0);
    if (ret == 0) {
        ts = (uint64_t) tv.tv_sec * 1000000U + (uint64_t) tv.tv_usec;
    }
    return ts;
}

#ifndef _WIN32
static ssize_t
safe_read(const int fd, void * const buf_, size_t count)
{
    unsigned char *buf = (unsigned char *) buf_;
    ssize_t        readnb;

    do {
        while ((readnb = read(fd, buf, count)) < (ssize_t) 0 &&
               errno == EINTR);
        if (readnb < (ssize_t) 0) {
            return readnb;
        }
        if (readnb == (ssize_t) 0) {
            break;
        }
        count -= (size_t) readnb;
        buf += readnb;
    } while (count > (ssize_t) 0);

    return (ssize_t) (buf - (unsigned char *) buf_);
}
#endif

#ifndef _WIN32
static int
randombytes_salsa20_random_random_dev_open(void)
{
    static const char * const devices[] = {
# ifndef USE_BLOCKING_RANDOM
        "/dev/arandom", "/dev/urandom",
# endif
        "/dev/random", NULL
    };
    const char * const *device = devices;

    do {
        if (access(*device, F_OK | R_OK) == 0) {
            return open(*device, O_RDONLY);
        }
        device++;
    } while (*device != NULL);

    return -1;
}

static void
randombytes_salsa20_random_init(void)
{
    stream.nonce = sodium_hrtime();
    assert(stream.nonce != (uint64_t) 0U);

    if ((stream.random_data_source_fd =
         randombytes_salsa20_random_random_dev_open()) == -1) {
        abort();
    }
}

#else /* _WIN32 */

static void
randombytes_salsa20_random_init(void)
{
    stream.nonce = sodium_hrtime();
    assert(stream.nonce != (uint64_t) 0U);

    if (! CryptAcquireContext(&stream.hcrypt_prov, NULL, NULL,
                              PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        abort();
    }
}
#endif

void
randombytes_salsa20_random_stir(void)
{
    const unsigned char s[crypto_auth_hmacsha512256_KEYBYTES] = {
        'T', 'h', 'i', 's', 'I', 's', 'J', 'u', 's', 't', 'A', 'T',
        'h', 'i', 'r', 't', 'y', 'T', 'w', 'o', 'B', 'y', 't', 'e',
        's', 'S', 'e', 'e', 'd', '.', '.', '.'
    };
    unsigned char  m0[crypto_auth_hmacsha512256_BYTES +
                      2U * SHA512_BLOCK_SIZE - SHA512_MIN_PAD_SIZE];
    unsigned char *k0 = m0 + crypto_auth_hmacsha512256_BYTES;
    size_t         i;
    size_t         sizeof_k0 = sizeof m0 - crypto_auth_hmacsha512256_BYTES;

    memset(stream.rnd32, 0, sizeof stream.rnd32);
    stream.rnd32_outleft = (size_t) 0U;
    if (stream.initialized == 0) {
        randombytes_salsa20_random_init();
        stream.initialized = 1;
    }
#ifndef _WIN32
    if (safe_read(stream.random_data_source_fd, m0,
                  sizeof m0) != (ssize_t) sizeof m0) {
        abort();
    }
#else /* _WIN32 */
    if (! CryptGenRandom(stream.hcrypt_prov, sizeof m0, m0)) {
        abort();
    }
#endif
    COMPILER_ASSERT(sizeof stream.key == crypto_auth_hmacsha512256_BYTES);
    crypto_auth_hmacsha512256(stream.key, k0, sizeof_k0, s);
    COMPILER_ASSERT(sizeof stream.key <= sizeof m0);
    for (i = (size_t) 0U; i < sizeof stream.key; i++) {
        stream.key[i] ^= m0[i];
    }
    sodium_memzero(m0, sizeof m0);
}

static void
randombytes_salsa20_random_stir_if_needed(void)
{
    const pid_t pid = getpid();

    if (stream.initialized == 0 || stream.pid != pid) {
        stream.pid = pid;
        randombytes_salsa20_random_stir();
    }
}

static uint32_t
randombytes_salsa20_random_getword(void)
{
    uint32_t val;
    int      ret;

    COMPILER_ASSERT(sizeof stream.rnd32 >= sizeof val);
    COMPILER_ASSERT(sizeof stream.rnd32 % sizeof val == (size_t) 0U);
    if (stream.rnd32_outleft <= (size_t) 0U) {
        randombytes_salsa20_random_stir_if_needed();
        COMPILER_ASSERT(sizeof stream.nonce == crypto_stream_salsa20_NONCEBYTES);
        ret = crypto_stream_salsa20((unsigned char *) stream.rnd32,
                                    (unsigned long long) sizeof stream.rnd32,
                                    (unsigned char *) &stream.nonce,
                                    stream.key);
        assert(ret == 0);
        stream.nonce++;
        stream.rnd32_outleft = sizeof stream.rnd32;
    }
    stream.rnd32_outleft -= sizeof val;
    memcpy(&val, &stream.rnd32[stream.rnd32_outleft], sizeof val);

    return val;
}

int
randombytes_salsa20_random_close(void)
{
    int ret = -1;

#ifndef _WIN32
    if (stream.random_data_source_fd != -1 &&
        close(stream.random_data_source_fd) == 0) {
        stream.random_data_source_fd = -1;
        stream.initialized = 0;
        ret = 0;
    }
#else /* _WIN32 */
    if (stream.initialized != 0 &&
        CryptReleaseContext(stream.hcrypt_prov, 0)) {
        stream.initialized = 0;
        ret = 0;
    }
#endif
    return ret;
}

uint32_t
randombytes_salsa20_random(void)
{
    return randombytes_salsa20_random_getword();
}

void
randombytes_salsa20_random_buf(void * const buf, const size_t size)
{
    int ret;

    randombytes_salsa20_random_stir_if_needed();
    COMPILER_ASSERT(sizeof stream.nonce == crypto_stream_salsa20_NONCEBYTES);
#ifdef ULONG_LONG_MAX
    assert(size <= ULONG_LONG_MAX);
#endif
    ret = crypto_stream_salsa20((unsigned char *) buf, (unsigned long long) size,
                                (unsigned char *) &stream.nonce,
                                stream.key);
    assert(ret == 0);
    stream.nonce++;
}

/*
 * randombytes_salsa20_random_uniform() derives from OpenBSD's arc4random_uniform()
 * Copyright (c) 2008, Damien Miller <djm@openbsd.org>
 */

uint32_t
randombytes_salsa20_random_uniform(const uint32_t upper_bound)
{
    uint32_t min;
    uint32_t r;

    if (upper_bound < 2) {
        return 0;
    }
    min = (uint32_t) (-upper_bound % upper_bound);
    for (;;) {
        r = randombytes_salsa20_random();
        if (r >= min) {
            break;
        }
    }
    return r % upper_bound;
}

const char *
randombytes_salsa20_implementation_name(void)
{
    return "salsa20";
}

struct randombytes_implementation randombytes_salsa20_implementation = {
    _SODIUM_C99(.implementation_name =) randombytes_salsa20_implementation_name,
    _SODIUM_C99(.random =) randombytes_salsa20_random,
    _SODIUM_C99(.stir =) randombytes_salsa20_random_stir,
    _SODIUM_C99(.uniform =) randombytes_salsa20_random_uniform,
    _SODIUM_C99(.buf =) randombytes_salsa20_random_buf,
    _SODIUM_C99(.close =) randombytes_salsa20_random_close
};
