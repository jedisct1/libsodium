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


int rand_sgx();

int rand_sgx()
{
    int ret;
    if (sgx_read_rand((unsigned char*)&ret, sizeof ret) != SGX_SUCCESS) {
        printf_enc("Error when reading rdrand in the SGX enclave\n");
    }
    // rand() has to return something between 0 and RAND_MAX
    // the following line does that, with a 2^-32 bias (on 0)
    // this is acceptable here as rand() is only used for testing
    return (ret>=0) ? ret : -ret;
}

int xmain(void);

void run_test(int* return_code)
{
    if (sodium_init() != 0) {
        *return_code = 99;
    }
    if (xmain() != 0) {
        *return_code = 99;
    }
}

#define main xmain
#define printf printf_enc

#define rand rand_sgx
#ifdef RAND_MAX
# undef RAND_MAX
#endif
#define RAND_MAX INT_MAX

#endif /* !_TEST_ENCLAVE_H_ */
