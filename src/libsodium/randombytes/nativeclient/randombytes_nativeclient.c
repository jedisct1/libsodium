#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "nacl/nacl_random.h"
#include "utils.h"
#include "randombytes.h"
#include "randombytes_nativeclient.h"

void
randombytes_nativeclient_random_stir(void)
{
}

int
randombytes_nativeclient_random_close(void)
{
    return 0;
}

uint32_t
randombytes_nativeclient_random(void)
{
    uint32_t r;

    randombytes_nativeclient_buf(&r, sizeof r);

    return r;
}

void
randombytes_nativeclient_buf(void * const buf, const size_t size)
{
    size_t readnb;

    if (nacl_secure_random(buf, size, &readnb) != 0) {
        abort();
    }

    assert(readnb == size);
}

const char *
randombytes_nativeclient_implementation_name(void)
{
    return "nativeclient";
}

struct randombytes_implementation randombytes_nativeclient_implementation = {
    SODIUM_C99(.implementation_name =) randombytes_nativeclient_implementation_name,
    SODIUM_C99(.random =) randombytes_nativeclient_random,
    SODIUM_C99(.stir =) randombytes_nativeclient_random_stir,
    SODIUM_C99(.uniform =) NULL,
    SODIUM_C99(.buf =) randombytes_nativeclient_buf,
    SODIUM_C99(.close =) randombytes_nativeclient_random_close
};
