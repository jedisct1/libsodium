/*
cpucycles monotonic.h version 20100803
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_monotonic_h
#define CPUCYCLES_monotonic_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_monotonic(void);
extern long long cpucycles_monotonic_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "monotonic"
#define cpucycles cpucycles_monotonic
#define cpucycles_persecond cpucycles_monotonic_persecond
#endif

#endif
