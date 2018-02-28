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

#endif /* !_TEST_ENCLAVE_H_ */
