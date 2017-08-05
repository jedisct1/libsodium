
#ifndef __CMPTEST_H__
#define __CMPTEST_H__

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "sodium.h"
#include "quirks.h"

#ifdef __EMSCRIPTEN__
# undef TEST_SRCDIR
# define TEST_SRCDIR "/test-data"
#endif
#ifndef TEST_SRCDIR
# define TEST_SRCDIR "."
#endif

#define TEST_NAME_RES TEST_NAME ".res"
#define TEST_NAME_OUT TEST_SRCDIR "/" TEST_NAME ".exp"

#ifdef HAVE_ARC4RANDOM
# undef rand
# define rand(X) arc4random(X)
#endif

int xmain(void);

#ifdef BENCHMARKS

# include <sys/time.h>

# ifndef ITERATIONS
#  define ITERATIONS 128
# endif

static unsigned long long now(void)
{
    struct             timeval tp;
    unsigned long long now;

    if (gettimeofday(&tp, NULL) != 0) {
        abort();
    }
    now = ((unsigned long long) tp.tv_sec * 1000000ULL) +
        (unsigned long long) tp.tv_usec;

    return now;
}

int main(void)
{
    unsigned long long ts_start;
    unsigned long long ts_end;
    unsigned int       i;

    if (sodium_init() != 0) {
        return 99;
    }
    randombytes_set_implementation(&randombytes_salsa20_implementation);
    ts_start = now();
    for (i = 0; i < ITERATIONS; i++) {
        if (xmain() != 0) {
            abort();
        }
    }
    ts_end = now();
    printf("%llu\n", 1000000ULL * (ts_end - ts_start) / ITERATIONS);

    return 0;
}

#define printf(...) do { } while(0)

#elif !defined(BROWSER_TESTS)

FILE *fp_res;

int main(void)
{
    FILE *fp_out;
    int   c;

    if ((fp_res = fopen(TEST_NAME_RES, "w+")) == NULL) {
        perror("fopen(" TEST_NAME_RES ")");
        return 99;
    }
    if (sodium_init() != 0) {
        return 99;
    }
    if (xmain() != 0) {
        return 99;
    }
    rewind(fp_res);
    if ((fp_out = fopen(TEST_NAME_OUT, "r")) == NULL) {
        perror("fopen(" TEST_NAME_OUT ")");
        return 99;
    }
    do {
        if ((c = fgetc(fp_res)) != fgetc(fp_out)) {
            return 99;
        }
    } while (c != EOF);

    return 0;
}

#undef  printf
#define printf(...) fprintf(fp_res, __VA_ARGS__)

#else

int main(void)
{
    if (sodium_init() != 0) {
        return 99;
    }
    if (xmain() != 0) {
        return 99;
    }
    printf("--- SUCCESS ---\n");

    return 0;
}

#endif

#define main xmain

#endif
