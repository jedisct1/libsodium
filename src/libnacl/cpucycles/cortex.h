/*
cpucycles cortex.h version 20100912
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_cortex_h
#define CPUCYCLES_cortex_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_cortex(void);
extern long long cpucycles_cortex_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "cortex"
#define cpucycles cpucycles_cortex
#define cpucycles_persecond cpucycles_cortex_persecond
#endif

#endif
