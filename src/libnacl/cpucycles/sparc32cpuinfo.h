/*
cpucycles sparc32cpuinfo.h version 20100804
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_sparc32cpuinfo_h
#define CPUCYCLES_sparc32cpuinfo_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_sparc32cpuinfo(void);
extern long long cpucycles_sparc32cpuinfo_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "sparc32cpuinfo"
#define cpucycles cpucycles_sparc32cpuinfo
#define cpucycles_persecond cpucycles_sparc32cpuinfo_persecond
#endif

#endif
