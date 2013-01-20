/*
cpucycles x86estimate.h version 20070121
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_x86estimate_h
#define CPUCYCLES_x86estimate_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_x86estimate(void);
extern long long cpucycles_x86estimate_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "x86estimate"
#define cpucycles cpucycles_x86estimate
#define cpucycles_persecond cpucycles_x86estimate_persecond
#endif

#endif
