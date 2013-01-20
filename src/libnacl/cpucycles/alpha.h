/*
cpucycles alpha.h version 20060318
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_alpha_h
#define CPUCYCLES_alpha_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_alpha(void);
extern long long cpucycles_alpha_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "alpha"
#define cpucycles cpucycles_alpha
#define cpucycles_persecond cpucycles_alpha_persecond
#endif

#endif
