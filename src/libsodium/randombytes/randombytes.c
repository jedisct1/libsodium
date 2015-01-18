
#ifdef __EMSCRIPTEN__
# include <emscripten.h>
#endif

#include <sys/types.h>

#include <assert.h>
#include <limits.h>
#include <stdint.h>

#include "randombytes.h"
#include "randombytes_sysrandom.h"

#ifdef __EMSCRIPTEN__
static const randombytes_implementation *implementation = NULL;
#else
static const randombytes_implementation *implementation =
    &randombytes_sysrandom_implementation;
#endif

int
randombytes_set_implementation(randombytes_implementation *impl)
{
    implementation = impl;

    return 0;
}

const char *
randombytes_implementation_name(void)
{
#ifdef __EMSCRIPTEN__
    return "js";
#else
    return implementation->implementation_name();
#endif
}

uint32_t
randombytes_random(void)
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT_V({
        return Module.getRandomValue();
    });
#else
    return implementation->random();
#endif
}

void
randombytes_stir(void)
{
#ifdef __EMSCRIPTEN__
    EM_ASM({
        if (Module.getRandomValue === undefined) {
            try {
                var randomValuesStandard = function() {
                    var buf = new Uint32Array(1);
                    window.crypto.getRandomValues(buf);
                    return buf[0] >>> 0;
                };
                randomValuesStandard();
                Module.getRandomValue = randomValuesStandard;
            } catch (e) {
                try {
                    var crypto = require('crypto');
                    var randomValueIOJS = function() {
                        var buf = crypto.randomBytes(4);
                        return (buf[0] << 24 | buf[1] << 16 || buf[2] << 8 || buf[3]) >>> 0;
                    };
                    randomValueIOJS();
                    Module.getRandomValue = randomValueIOJS;
                } catch (e) {
                    throw 'No secure random number generator found';
                }
            }
        }
    });
#else
    implementation->stir();
#endif
}

uint32_t
randombytes_uniform(const uint32_t upper_bound)
{
#ifdef __EMSCRIPTEN__
    uint32_t min;
    uint32_t r;

    if (upper_bound < 2) {
        return 0;
    }
    min = (uint32_t) (-upper_bound % upper_bound);
    do {
        r = randombytes_random();
    } while (r < min);

    return r % upper_bound;
#else
    return implementation->uniform(upper_bound);
#endif
}

void
randombytes_buf(void * const buf, const size_t size)
{
#ifdef __EMSCRIPTEN__
    unsigned char *p = buf;
    size_t         i;

    for (i = (size_t) 0U; i < size; i++) {
        p[i] = (unsigned char) randombytes_random();
    }
#else
    if (size > (size_t) 0U) {
        implementation->buf(buf, size);
    }
#endif
}

int
randombytes_close(void)
{
#ifdef __EMSCRIPTEN__
    return 0;
#else
    return implementation->close();
#endif
}

void
randombytes(unsigned char * const buf, const unsigned long long buf_len)
{
    assert(buf_len <= SIZE_MAX);
    randombytes_buf(buf, (size_t) buf_len);
}
