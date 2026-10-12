/* Adapted from amd64-51/fe25519.h. */

#ifndef fe51_H
#define fe51_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

#include "fe51_namespace.h"

typedef struct
{
    uint64_t v[5];
}
fe51;

extern void fe51_pack(unsigned char *, const fe51 *);
extern void fe51_mul(fe51 *, const fe51 *, const fe51 *);
extern void fe51_invert(fe51 *, const fe51 *);

#ifdef __cplusplus
}
#endif

#endif
