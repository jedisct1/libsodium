/*
cpucycles monotoniccpuinfo.h version 20100804
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_monotoniccpuinfo_h
#define CPUCYCLES_monotoniccpuinfo_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_monotoniccpuinfo(void);
extern long long cpucycles_monotoniccpuinfo_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "monotoniccpuinfo"
#define cpucycles cpucycles_monotoniccpuinfo
#define cpucycles_persecond cpucycles_monotoniccpuinfo_persecond
#endif

#endif
