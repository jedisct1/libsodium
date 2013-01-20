/*
cpucycles mips.h version 20100802
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_mips_h
#define CPUCYCLES_mips_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_mips(void);
extern long long cpucycles_mips_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "mips"
#define cpucycles cpucycles_mips
#define cpucycles_persecond cpucycles_mips_persecond
#endif

#endif
