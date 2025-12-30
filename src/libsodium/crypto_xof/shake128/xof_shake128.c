#include "crypto_xof_shake128.h"
#include "private/common.h"
#include "ref/shake128_ref.h"

size_t
crypto_xof_shake128_blockbytes(void)
{
    return crypto_xof_shake128_BLOCKBYTES;
}

size_t
crypto_xof_shake128_statebytes(void)
{
    return crypto_xof_shake128_STATEBYTES;
}

unsigned char
crypto_xof_shake128_domain_standard(void)
{
    return crypto_xof_shake128_DOMAIN_STANDARD;
}

int
crypto_xof_shake128(unsigned char *out, size_t outlen, const unsigned char *in,
                    unsigned long long inlen)
{
    COMPILER_ASSERT(sizeof(crypto_xof_shake128_state) >= sizeof(shake128_state_internal));

    return shake128_ref(out, outlen, in, (size_t) inlen);
}

int
crypto_xof_shake128_init(crypto_xof_shake128_state *state)
{
    shake128_state_internal *st = (shake128_state_internal *) (void *) state;

    COMPILER_ASSERT(sizeof(crypto_xof_shake128_state) >= sizeof(shake128_state_internal));

    return shake128_ref_init(st);
}

int
crypto_xof_shake128_init_with_domain(crypto_xof_shake128_state *state, unsigned char domain)
{
    shake128_state_internal *st = (shake128_state_internal *) (void *) state;

    COMPILER_ASSERT(sizeof(crypto_xof_shake128_state) >= sizeof(shake128_state_internal));

    return shake128_ref_init_with_domain(st, domain);
}

int
crypto_xof_shake128_update(crypto_xof_shake128_state *state,
                           const unsigned char       *in,
                           unsigned long long         inlen)
{
    shake128_state_internal *st = (shake128_state_internal *) (void *) state;

    return shake128_ref_update(st, in, (size_t) inlen);
}

int
crypto_xof_shake128_squeeze(crypto_xof_shake128_state *state, unsigned char *out, size_t outlen)
{
    shake128_state_internal *st = (shake128_state_internal *) (void *) state;

    return shake128_ref_squeeze(st, out, outlen);
}
