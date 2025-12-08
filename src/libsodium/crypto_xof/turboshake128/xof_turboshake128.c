#include "crypto_xof_turboshake128.h"
#include "private/common.h"
#include "ref/turboshake128_ref.h"

size_t
crypto_xof_turboshake128_blockbytes(void)
{
    return crypto_xof_turboshake128_BLOCKBYTES;
}

size_t
crypto_xof_turboshake128_statebytes(void)
{
    return crypto_xof_turboshake128_STATEBYTES;
}

unsigned char
crypto_xof_turboshake128_domain_standard(void)
{
    return crypto_xof_turboshake128_DOMAIN_STANDARD;
}

int
crypto_xof_turboshake128(unsigned char *out, size_t outlen, const unsigned char *in,
                         unsigned long long inlen)
{
    COMPILER_ASSERT(sizeof(crypto_xof_turboshake128_state) >= sizeof(turboshake128_state_internal));

    return turboshake128_ref(out, outlen, in, (size_t) inlen);
}

int
crypto_xof_turboshake128_init(crypto_xof_turboshake128_state *state)
{
    turboshake128_state_internal *st = (turboshake128_state_internal *) (void *) state;

    COMPILER_ASSERT(sizeof(crypto_xof_turboshake128_state) >= sizeof(turboshake128_state_internal));

    return turboshake128_ref_init(st);
}

int
crypto_xof_turboshake128_init_with_domain(crypto_xof_turboshake128_state *state,
                                          unsigned char                   domain)
{
    turboshake128_state_internal *st = (turboshake128_state_internal *) (void *) state;

    COMPILER_ASSERT(sizeof(crypto_xof_turboshake128_state) >= sizeof(turboshake128_state_internal));

    return turboshake128_ref_init_with_domain(st, domain);
}

int
crypto_xof_turboshake128_update(crypto_xof_turboshake128_state *state,
                                const unsigned char            *in,
                                unsigned long long              inlen)
{
    turboshake128_state_internal *st = (turboshake128_state_internal *) (void *) state;

    return turboshake128_ref_update(st, in, (size_t) inlen);
}

int
crypto_xof_turboshake128_squeeze(crypto_xof_turboshake128_state *state, unsigned char *out,
                                 size_t outlen)
{
    turboshake128_state_internal *st = (turboshake128_state_internal *) (void *) state;

    return turboshake128_ref_squeeze(st, out, outlen);
}
