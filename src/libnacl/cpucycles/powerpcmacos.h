/*
cpucycles powerpcmacos.h version 20060319
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_powerpcmacos_h
#define CPUCYCLES_powerpcmacos_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_powerpcmacos(void);
extern long long cpucycles_powerpcmacos_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "powerpcmacos"
#define cpucycles cpucycles_powerpcmacos
#define cpucycles_persecond cpucycles_powerpcmacos_persecond
#endif

#endif
