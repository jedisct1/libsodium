
#include <stdlib.h>
#include <sys/types.h>
#ifndef _WIN32
# include <sys/stat.h>
# include <sys/time.h>
#endif
#ifdef __linux__
# include <sys/syscall.h>
# include <poll.h>
#endif

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#if !defined(_MSC_VER) && !defined(__BORLANDC__)
# include <unistd.h>
#endif

#include "crypto_core_salsa20.h"
#include "crypto_generichash.h"
#include "crypto_stream_salsa20.h"
#include "randombytes.h"
#include "randombytes_salsa20_random.h"
#include "utils.h"
#include "private/mutex.h"

#ifdef _WIN32
# include <windows.h>
# include <sys/timeb.h>
# define RtlGenRandom SystemFunction036
# if defined(__cplusplus)
extern "C"
# endif
BOOLEAN NTAPI RtlGenRandom(PVOID RandomBuffer, ULONG RandomBufferLength);
# pragma comment(lib, "advapi32.lib")
# ifdef __BORLANDC__
#  define _ftime ftime
#  define _timeb timeb
# endif
#endif

#define SALSA20_RANDOM_BLOCK_SIZE crypto_core_salsa20_OUTPUTBYTES
#define HASH_BLOCK_SIZE 128U
#define COMPILER_ASSERT(X) (void) sizeof(char[(X) ? 1 : -1])

#if defined(__OpenBSD__) || defined(__CloudABI__)
# define HAVE_SAFE_ARC4RANDOM 1
#endif

#ifndef SSIZE_MAX
# define SSIZE_MAX (SIZE_MAX / 2 - 1)
#endif

typedef struct Salsa20Random_ {
    size_t        rnd32_outleft;
    int           random_data_source_fd;
    int           initialized;
    int           getrandom_available;
    unsigned char key[crypto_stream_salsa20_KEYBYTES];
    unsigned char rnd32[16U * SALSA20_RANDOM_BLOCK_SIZE];
    uint64_t      nonce;
#ifdef HAVE_GETPID
    pid_t         pid;
#endif
} Salsa20Random;

static Salsa20Random stream = {
    SODIUM_C99(.rnd32_outleft =) (size_t) 0U,
    SODIUM_C99(.random_data_source_fd =) -1,
    SODIUM_C99(.initialized =) 0,
    SODIUM_C99(.getrandom_available =) 0
};

static uint64_t
sodium_hrtime(void)
{
    uint64_t ts;

#ifdef _WIN32
    {
        struct _timeb tb;
# pragma warning(push)
# pragma warning(disable: 4996)
        _ftime(&tb);
# pragma warning(pop)
        ts = ((uint64_t) tb.time) * 1000000U + ((uint64_t) tb.millitm) * 1000U;
    }
#else
    {
        struct timeval tv;

        if (gettimeofday(&tv, NULL) != 0) {
            abort(); /* LCOV_EXCL_LINE */
        }
        ts = ((uint64_t) tv.tv_sec) * 1000000U + (uint64_t) tv.tv_usec;
    }
#endif
    return ts;
}

#ifndef _WIN32
static ssize_t
safe_read(const int fd, void * const buf_, size_t size)
{
    unsigned char *buf = (unsigned char *) buf_;
    ssize_t        readnb;

    assert(size > (size_t) 0U);
    assert(size <= SSIZE_MAX);
    do {
        while ((readnb = read(fd, buf, size)) < (ssize_t) 0 &&
               (errno == EINTR || errno == EAGAIN)); /* LCOV_EXCL_LINE */
        if (readnb < (ssize_t) 0) {
            return readnb; /* LCOV_EXCL_LINE */
        }
        if (readnb == (ssize_t) 0) {
            break; /* LCOV_EXCL_LINE */
        }
        size -= (size_t) readnb;
        buf += readnb;
    } while (size > (ssize_t) 0);

    return (ssize_t) (buf - (unsigned char *) buf_);
}
#endif

#ifndef _WIN32
# if defined(__linux__) && !defined(USE_BLOCKING_RANDOM) && !defined(NO_BLOCKING_RANDOM_POLL)
static int
randombytes_block_on_dev_random(void)
{
    struct pollfd pfd;
    int           fd;
    int           pret;

    fd = open("/dev/random", O_RDONLY);
    if (fd == -1) {
        return 0;
    }
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    do {
        pret = poll(&pfd, 1, -1);
    } while (pret < 0 && (errno == EINTR || errno == EAGAIN));
    if (pret != 1) {
        (void) close(fd);
        errno = EIO;
        return -1;
    }
    return close(fd);
}
# endif

# ifndef HAVE_SAFE_ARC4RANDOM
static int
randombytes_salsa20_random_random_dev_open(void)
{
/* LCOV_EXCL_START */
    struct stat       st;
    static const char *devices[] = {
#  ifndef USE_BLOCKING_RANDOM
        "/dev/urandom",
#  endif
        "/dev/random", NULL
    };
    const char **     device = devices;
    int               fd;

# if defined(__linux__) && !defined(USE_BLOCKING_RANDOM) && !defined(NO_BLOCKING_RANDOM_POLL)
    if (randombytes_block_on_dev_random() != 0) {
        return -1;
    }
# endif
    do {
        fd = open(*device, O_RDONLY);
        if (fd != -1) {
            if (fstat(fd, &st) == 0 &&
# ifdef __COMPCERT__
                1
# elif defined(S_ISNAM)
                (S_ISNAM(st.st_mode) || S_ISCHR(st.st_mode))
# else
                S_ISCHR(st.st_mode)
# endif
               ) {
#  if defined(F_SETFD) && defined(FD_CLOEXEC)
                (void) fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
#  endif
                return fd;
            }
            (void) close(fd);
        } else if (errno == EINTR) {
            continue;
        }
        device++;
    } while (*device != NULL);

    errno = EIO;
    return -1;
/* LCOV_EXCL_STOP */
}
# endif

# if defined(SYS_getrandom) && defined(__NR_getrandom)
static int
_randombytes_linux_getrandom(void * const buf, const size_t size)
{
    int readnb;

    assert(size <= 256U);
    do {
        readnb = syscall(SYS_getrandom, buf, (int) size, 0);
    } while (readnb < 0 && (errno == EINTR || errno == EAGAIN));

    return (readnb == (int) size) - 1;
}

static int
randombytes_linux_getrandom(void * const buf_, size_t size)
{
    unsigned char *buf = (unsigned char *) buf_;
    size_t         chunk_size = 256U;

    do {
        if (size < chunk_size) {
            chunk_size = size;
            assert(chunk_size > (size_t) 0U);
        }
        if (_randombytes_linux_getrandom(buf, chunk_size) != 0) {
            return -1;
        }
        size -= chunk_size;
        buf += chunk_size;
    } while (size > (size_t) 0U);

    return 0;
}
# endif

static void
randombytes_salsa20_random_init(void)
{
    const int errno_save = errno;

    stream.nonce = sodium_hrtime();
    assert(stream.nonce != (uint64_t) 0U);

# ifdef HAVE_SAFE_ARC4RANDOM
    errno = errno_save;
# else

#  if defined(SYS_getrandom) && defined(__NR_getrandom)
    {
        unsigned char fodder[16];

        if (randombytes_linux_getrandom(fodder, sizeof fodder) == 0) {
            stream.getrandom_available = 1;
            errno = errno_save;
            return;
        }
        stream.getrandom_available = 0;
    }
#  endif /* SYS_getrandom */

    if ((stream.random_data_source_fd =
         randombytes_salsa20_random_random_dev_open()) == -1) {
        abort(); /* LCOV_EXCL_LINE */
    }
    errno = errno_save;
# endif /* HAVE_SAFE_ARC4RANDOM */
}

#else /* _WIN32 */

static void
randombytes_salsa20_random_init(void)
{
    stream.nonce = sodium_hrtime();
    assert(stream.nonce != (uint64_t) 0U);
}
#endif

static void
randombytes_salsa20_random_rekey(const unsigned char * const mix)
{
    unsigned char *key = stream.key;
    size_t         i;

    for (i = (size_t) 0U; i < sizeof stream.key; i++) {
        key[i] ^= mix[i];
    }
}

static void
randombytes_salsa20_random_stir_unlocked(void)
{
    /* constant to personalize the hash function */
    const unsigned char hsigma[crypto_generichash_KEYBYTES] = {
        0x54, 0x68, 0x69, 0x73, 0x49, 0x73, 0x4a, 0x75,
        0x73, 0x74, 0x41, 0x54, 0x68, 0x69, 0x72, 0x74,
        0x79, 0x54, 0x77, 0x6f, 0x42, 0x79, 0x74, 0x65,
        0x73, 0x53, 0x65, 0x65, 0x64, 0x2e, 0x2e, 0x2e
    };
    unsigned char  m0[crypto_stream_salsa20_KEYBYTES + HASH_BLOCK_SIZE];
    unsigned char *k0 = m0 + crypto_stream_salsa20_KEYBYTES;
    size_t         sizeof_k0 = sizeof m0 - crypto_stream_salsa20_KEYBYTES;

    memset(stream.rnd32, 0, sizeof stream.rnd32);
    stream.rnd32_outleft = (size_t) 0U;
    if (stream.initialized == 0) {
        randombytes_salsa20_random_init();
        stream.initialized = 1;
    }
#ifndef _WIN32

# ifdef HAVE_SAFE_ARC4RANDOM
    arc4random_buf(m0, sizeof m0);
# elif defined(SYS_getrandom) && defined(__NR_getrandom)
    if (stream.getrandom_available != 0) {
        if (randombytes_linux_getrandom(m0, sizeof m0) != 0) {
            abort(); /* LCOV_EXCL_LINE */
        }
    } else if (stream.random_data_source_fd == -1 ||
               safe_read(stream.random_data_source_fd, m0,
                         sizeof m0) != (ssize_t) sizeof m0) {
        abort(); /* LCOV_EXCL_LINE */
    }
# else
    if (stream.random_data_source_fd == -1 ||
        safe_read(stream.random_data_source_fd, m0,
                  sizeof m0) != (ssize_t) sizeof m0) {
        abort(); /* LCOV_EXCL_LINE */
    }
# endif

#else /* _WIN32 */
    if (! RtlGenRandom((PVOID) m0, (ULONG) sizeof m0)) {
        abort(); /* LCOV_EXCL_LINE */
    }
#endif
    if (crypto_generichash(stream.key, sizeof stream.key, k0, sizeof_k0,
                           hsigma, sizeof hsigma) != 0) {
        abort(); /* LCOV_EXCL_LINE */
    }
    COMPILER_ASSERT(sizeof stream.key <= sizeof m0);
    randombytes_salsa20_random_rekey(m0);
    sodium_memzero(m0, sizeof m0);
#ifdef HAVE_GETPID
    stream.pid = getpid();
#endif
}

static void
randombytes_salsa20_random_stir(void)
{
    if (sodium_crit_enter() != 0) {
        abort();
    }
    randombytes_salsa20_random_stir_unlocked();
    if (sodium_crit_leave() != 0) {
        abort();
    }
}

static void
randombytes_salsa20_random_stir_if_needed(void)
{
#ifdef HAVE_GETPID
    if (stream.initialized == 0) {
        randombytes_salsa20_random_stir_unlocked();
    } else if (stream.pid != getpid()) {
        abort();
    }
#else
    if (stream.initialized == 0) {
        randombytes_salsa20_random_stir_unlocked();
    }
#endif
}

static int
randombytes_salsa20_random_close(void)
{
    int ret = -1;

    if (sodium_crit_enter() != 0) {
        abort();
    }
#ifndef _WIN32
    if (stream.random_data_source_fd != -1 &&
        close(stream.random_data_source_fd) == 0) {
        stream.random_data_source_fd = -1;
        stream.initialized = 0;
# ifdef HAVE_GETPID
        stream.pid = (pid_t) 0;
# endif
        ret = 0;
    }

# ifdef HAVE_SAFE_ARC4RANDOM
    ret = 0;
# endif

# if defined(SYS_getrandom) && defined(__NR_getrandom)
    if (stream.getrandom_available != 0) {
        ret = 0;
    }
# endif

#else /* _WIN32 */
    if (stream.initialized != 0) {
        stream.initialized = 0;
        ret = 0;
    }
#endif
    if (sodium_crit_leave() != 0) {
        abort();
    }
    return ret;
}

static void
randombytes_salsa20_random_buf(void * const buf, const size_t size)
{
    size_t i;
    int    ret;

    if (sodium_crit_enter() != 0) {
        abort();
    }
    randombytes_salsa20_random_stir_if_needed();
    COMPILER_ASSERT(sizeof stream.nonce == crypto_stream_salsa20_NONCEBYTES);
#ifdef ULONG_LONG_MAX
    /* coverity[result_independent_of_operands] */
    assert(size <= ULONG_LONG_MAX);
#endif
    ret = crypto_stream_salsa20((unsigned char *) buf, (unsigned long long) size,
                                (unsigned char *) &stream.nonce, stream.key);
    assert(ret == 0);
    for (i = 0U; i < sizeof size; i++) {
        stream.key[i] ^= ((const unsigned char *) (const void *) &size)[i];
    }
    stream.nonce++;
    crypto_stream_salsa20_xor(stream.key, stream.key, sizeof stream.key,
                              (unsigned char *) &stream.nonce, stream.key);
    if (sodium_crit_leave() != 0) {
        abort();
    }
}

static uint32_t
randombytes_salsa20_random(void)
{
    uint32_t val;
    int      ret;

    if (sodium_crit_enter() != 0) {
        abort();
    }
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
    if (sodium_crit_leave() != 0) {
        abort();
    }
    return val;
}

static const char *
randombytes_salsa20_implementation_name(void)
{
    return "salsa20";
}

struct randombytes_implementation randombytes_salsa20_implementation = {
    SODIUM_C99(.implementation_name =) randombytes_salsa20_implementation_name,
    SODIUM_C99(.random =) randombytes_salsa20_random,
    SODIUM_C99(.stir =) randombytes_salsa20_random_stir,
    SODIUM_C99(.uniform =) NULL,
    SODIUM_C99(.buf =) randombytes_salsa20_random_buf,
    SODIUM_C99(.close =) randombytes_salsa20_random_close
};
