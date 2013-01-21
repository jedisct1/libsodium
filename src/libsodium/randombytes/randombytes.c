
#include <sys/types.h>

#include <assert.h>
#include <limits.h>

#include "randombytes.h"
#include "salsa20_random.h"

void
randombytes(unsigned char * const buf, const unsigned long long buf_len)
{
    assert(buf_len <= SIZE_MAX);
    salsa20_random_buf(buf, buf_len);
}
