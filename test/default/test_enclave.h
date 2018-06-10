#ifndef _TEST_ENCLAVE_H_
#define _TEST_ENCLAVE_H_

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "enclave_t.h"  /* print_string */
#include "sgx_trts.h"

#include "sodium.h"

void printf_enc(const char *fmt, ...);

#ifndef BUFSIZ
#define BUFSIZ 1024
#endif

void printf_enc(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
}

#undef  printf
#define printf printf_enc

void log_string(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_log_string(buf);
}

int rand_sgx();

int rand_sgx()
{
    int ret;
    if (sgx_read_rand((unsigned char*)&ret, sizeof ret) != SGX_SUCCESS) {
        printf_enc("Error when reading rdrand in the SGX enclave\n");
        log_string("Enclave: Error when reading rdrand in the SGX enclave\n");
    }
    // rand() has to return something between 0 and RAND_MAX
    // the following line does that, with a 2^-32 bias (on 0)
    // this is acceptable here as rand() is only used for testing
    return (ret>=0) ? ret : -ret;
}


struct {
    void   *pnt;
    size_t  size;
} mempool[1024];

static size_t mempool_idx;

static __attribute__((malloc)) void *mempool_alloc(size_t size)
{
    size_t i;
    if (size >= (size_t) 0x80000000 - (size_t) 0x00000fff) {
        return NULL;
    }
    size = (size + (size_t) 0x00000fff) & ~ (size_t) 0x00000fff;
    for (i = 0U; i < mempool_idx; i++) {
        if (mempool[i].size >= (size | (size_t) 0x80000000)) {
            mempool[i].size &= ~ (size_t) 0x80000000;
            return mempool[i].pnt;
        }
    }
    if (mempool_idx >= sizeof mempool / sizeof mempool[0]) {
        return NULL;
    }
    mempool[mempool_idx].size = size;
    return (mempool[mempool_idx++].pnt = (void *) malloc(size));
}

static void mempool_free(void *pnt)
{
    size_t i;
    for (i = 0U; i < mempool_idx; i++) {
        if (mempool[i].pnt == pnt) {
            if ((mempool[i].size & (size_t) 0x80000000) != (size_t) 0x0) {
                break;
            }
            mempool[i].size |= (size_t) 0x80000000;
            return;
        }
    }
    abort();
}

static __attribute__((malloc)) void *mempool_allocarray(size_t count, size_t size)
{
    if (count > (size_t) 0U && size >= (size_t) SIZE_MAX / count) {
        return NULL;
    }
    return mempool_alloc(count * size);
}

static int mempool_free_all(void)
{
    size_t i;
    int    ret = 0;

    for (i = 0U; i < mempool_idx; i++) {
        if ((mempool[i].size & (size_t) 0x80000000) == (size_t) 0x0) {
            ret = -1;
        }
        free(mempool[i].pnt);
        mempool[i].pnt = NULL;
    }
    mempool_idx = (size_t) 0U;

    return ret;
}

#undef sodium_malloc 
#undef sodium_free
#undef sodium_allocarray

#define sodium_malloc(X)        mempool_alloc(X)
#define sodium_free(X)          mempool_free(X)
#define sodium_allocarray(X, Y) mempool_allocarray((X), (Y))


int xmain(void);

void run_test(int* return_code)
{
    log_string("Enclave: run_test().\n");
    log_string("Enclave: Initialize libsodium.\n");
    if (sodium_init() != 0) {
        log_string("Enclave: libsodium's init failed.\n");
        *return_code = 99;
    }else{
        log_string("Enclave: libsodium's init succeeded.\n");
        log_string("Enclave: call the test function.\n");
        if (xmain() != 0) {
            log_string("Enclave: test failed.\n");
            *return_code = 99;
        }else{
            log_string("Enclave: test succeeded.\n");
        }
        
        if (mempool_free_all() != 0) {
            log_string("** Enclave: memory leaks detected **\n");
            *return_code = 99;
        }
    }
}

#define main xmain


#undef rand
#define rand rand_sgx

#ifdef RAND_MAX
# undef RAND_MAX
#endif
#define RAND_MAX INT_MAX

#endif /* !_TEST_ENCLAVE_H_ */
