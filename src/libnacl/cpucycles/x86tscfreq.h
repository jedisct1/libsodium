/*
cpucycles x86tscfreq.h version 20060318
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_x86tscfreq_h
#define CPUCYCLES_x86tscfreq_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_x86tscfreq(void);
extern long long cpucycles_x86tscfreq_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "x86tscfreq"
#define cpucycles cpucycles_x86tscfreq
#define cpucycles_persecond cpucycles_x86tscfreq_persecond
#endif

#endif
