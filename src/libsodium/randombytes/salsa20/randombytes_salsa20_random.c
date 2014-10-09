
#include <sys/types.h>
#ifndef _WIN32
# include <sys/stat.h>
# include <sys/time.h>
#endif

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifndef _MSC_VER
# include <unistd.h>
#endif

#include "crypto_core_salsa20.h"
#include "crypto_auth_hmacsha512256.h"
#include "crypto_stream_salsa20.h"
#include "randombytes.h"
#include "randombytes_salsa20_random.h"
#include "utils.h"

#ifdef _WIN32
# include <windows.h>
# include <sys/timeb.h>
# define RtlGenRandom SystemFunction036
# if defined(__cplusplus)
extern "C"
# endif
BOOLEAN NTAPI RtlGenRandom(PVOID RandomBuffer, ULONG RandomBufferLength);
# pragma comment(lib, "advapi32.lib")
#endif

#define SALSA20_RANDOM_BLOCK_SIZE crypto_core_salsa20_OUTPUTBYTES
#define SHA512_BLOCK_SIZE 128U
#define SHA512_MIN_PAD_SIZE (1U + 16U)
#define COMPILER_ASSERT(X) (void) sizeof(char[(X) ? 1 : -1])

typedef struct Salsa20Random_ {
    unsigned char key[crypto_stream_salsa20_KEYBYTES];
    unsigned char rnd32[16U * SALSA20_RANDOM_BLOCK_SIZE];
    uint64_t      nonce;
    size_t        rnd32_outleft;
#ifndef _MSC_VER
    pid_t         pid;
#endif
    int           random_data_source_fd;
    int           initialized;
} Salsa20Random;

static Salsa20Random stream = {
    SODIUM_C99(.random_data_source_fd =) -1,
    SODIUM_C99(.rnd32_outleft =) (size_t) 0U,
    SODIUM_C99(.initialized =) 0
};

static uint64_t
sodium_hrtime(void)
{
    struct timeval tv;
    uint64_t       ts = (uint64_t) 0U;
    int            ret;

#ifdef _WIN32
    struct _timeb tb;

# pragma warning(push)
# pragma warning(disable: 4996)
    _ftime(&tb);
# pragma warning(pop)
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

    assert(count > (size_t) 0U);
    do {
        while ((readnb = read(fd, buf, count)) < (ssize_t) 0 &&
               errno == EINTR);  /* LCOV_EXCL_LINE */
        if (readnb < (ssize_t) 0) {
            return readnb; /* LCOV_EXCL_LINE */
        }
        if (readnb == (ssize_t) 0) {
            break; /* LCOV_EXCL_LINE */
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
/* LCOV_EXCL_START */
    struct stat       st;
    static const char *devices[] = {
# ifndef USE_BLOCKING_RANDOM
        "/dev/urandom",
# endif
        "/dev/random", NULL
    };
    const char **     device = devices;
    int               fd;

    do {
        if ((fd = open(*device, O_RDONLY)) != -1) {
            if (fstat(fd, &st) == 0 && S_ISCHR(st.st_mode)) {
                return fd;
            }
            (void) close(fd);
        }
        device++;
    } while (*device != NULL);

    return -1;
/* LCOV_EXCL_STOP */
}

static void
randombytes_salsa20_random_init(void)
{
    const int errno_save = errno;

    stream.nonce = sodium_hrtime();
    assert(stream.nonce != (uint64_t) 0U);

    if ((stream.random_data_source_fd =
         randombytes_salsa20_random_random_dev_open()) == -1) {
        abort(); /* LCOV_EXCL_LINE */
    }
    errno = errno_save;
}

#else /* _WIN32 */

static void
randombytes_salsa20_random_init(void)
{
    stream.nonce = sodium_hrtime();
    assert(stream.nonce != (uint64_t) 0U);
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
        abort(); /* LCOV_EXCL_LINE */
    }
#else /* _WIN32 */
    if (! RtlGenRandom((PVOID) m0, (ULONG) sizeof m0)) {
        abort(); /* LCOV_EXCL_LINE */
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
#ifdef _MSC_VER
    if (stream.initialized == 0) {
        randombytes_salsa20_random_stir();
    }
#else
    const pid_t pid = getpid();

    if (stream.initialized == 0 || stream.pid != pid) {
        stream.pid = pid;
        randombytes_salsa20_random_stir();
    }
#endif
}

static void
randombytes_salsa20_random_rekey(const unsigned char * const mix)
{
    unsigned char *key = stream.key;
    size_t         i;

    for (i = (size_t) 0U; i < sizeof stream.key; i++) {
        key[i] ^= mix[i];
    }
}

static uint32_t
randombytes_salsa20_random_getword(void)
{
    uint32_t val;
    int      ret;

    COMPILER_ASSERT(sizeof stream.rnd32 >= (sizeof stream.key) + (sizeof val));
    COMPILER_ASSERT(((sizeof stream.rnd32) - (sizeof stream.key))
                    % sizeof val == (size_t) 0U);
    if (stream.rnd32_outleft <= (size_t) 0U) {
        randombytes_salsa20_random_stir_if_needed();
        COMPILER_ASSERT(sizeof stream.nonce == crypto_stream_salsa20_NONCEBYTES);
        ret = crypto_stream_salsa20((unsigned char *) stream.rnd32,
                                    (unsigned long long) sizeof stream.rnd32,
                                    (unsigned char *) &stream.nonce,
                                    stream.key);
        assert(ret == 0);
        stream.rnd32_outleft = (sizeof stream.rnd32) - (sizeof stream.key);
        randombytes_salsa20_random_rekey(&stream.rnd32[stream.rnd32_outleft]);
        stream.nonce++;
    }
    stream.rnd32_outleft -= sizeof val;
    memcpy(&val, &stream.rnd32[stream.rnd32_outleft], sizeof val);
    memset(&stream.rnd32[stream.rnd32_outleft], 0, sizeof val);

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
    if (stream.initialized != 0) {
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
    /* coverity[result_independent_of_operands] */
    assert(size <= ULONG_LONG_MAX);
#endif
    ret = crypto_stream_salsa20((unsigned char *) buf, (unsigned long long) size,
                                (unsigned char *) &stream.nonce, stream.key);
    assert(ret == 0);
    stream.nonce++;
    crypto_stream_salsa20_xor(stream.key, stream.key, sizeof stream.key,
                              (unsigned char *) &stream.nonce, stream.key);
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
    } /* LCOV_EXCL_LINE */
    return r % upper_bound;
}

const char *
randombytes_salsa20_implementation_name(void)
{
    return "salsa20";
}

struct randombytes_implementation randombytes_salsa20_implementation = {
    SODIUM_C99(.implementation_name =) randombytes_salsa20_implementation_name,
    SODIUM_C99(.random =) randombytes_salsa20_random,
    SODIUM_C99(.stir =) randombytes_salsa20_random_stir,
    SODIUM_C99(.uniform =) randombytes_salsa20_random_uniform,
    SODIUM_C99(.buf =) randombytes_salsa20_random_buf,
    SODIUM_C99(.close =) randombytes_salsa20_random_close
};
