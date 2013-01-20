/*
cpucycles gettimeofday.h version 20060318
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_gettimeofday_h
#define CPUCYCLES_gettimeofday_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_gettimeofday(void);
extern long long cpucycles_gettimeofday_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "gettimeofday"
#define cpucycles cpucycles_gettimeofday
#define cpucycles_persecond cpucycles_gettimeofday_persecond
#endif

#endif
