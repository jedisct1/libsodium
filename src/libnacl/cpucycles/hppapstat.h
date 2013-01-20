/*
cpucycles hppapstat.h version 20060319
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_hppapstat_h
#define CPUCYCLES_hppapstat_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_hppapstat(void);
extern long long cpucycles_hppapstat_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "hppapstat"
#define cpucycles cpucycles_hppapstat
#define cpucycles_persecond cpucycles_hppapstat_persecond
#endif

#endif
