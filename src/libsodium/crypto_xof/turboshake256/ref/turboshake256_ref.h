#ifndef turboshake256_ref_H
#define turboshake256_ref_H

#include <stddef.h>
#include <stdint.h>

#define TURBOSHAKE256_RATE 136

typedef enum {
    TURBOSHAKE256_PHASE_ABSORBING = 0,
    TURBOSHAKE256_PHASE_SQUEEZING = 1
} turboshake256_phase;

typedef struct turboshake256_state_internal_ {
    unsigned char state[200];
    size_t        offset;
    uint8_t       phase;
    unsigned char domain; /* Domain separation byte */
} turboshake256_state_internal;

int turboshake256_ref(unsigned char *out, size_t outlen, const unsigned char *in, size_t inlen);

int turboshake256_ref_init(turboshake256_state_internal *state);

int turboshake256_ref_init_with_domain(turboshake256_state_internal *state, unsigned char domain);

int turboshake256_ref_update(turboshake256_state_internal *state,
                             const unsigned char          *in,
                             size_t                        inlen);

int turboshake256_ref_squeeze(turboshake256_state_internal *state, unsigned char *out,
                              size_t outlen);

#endif /* turboshake256_ref_H */
