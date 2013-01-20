/*
cpucycles ia64cpuinfo.h version 20100803
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_ia64cpuinfo_h
#define CPUCYCLES_ia64cpuinfo_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_ia64cpuinfo(void);
extern long long cpucycles_ia64cpuinfo_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "ia64cpuinfo"
#define cpucycles cpucycles_ia64cpuinfo
#define cpucycles_persecond cpucycles_ia64cpuinfo_persecond
#endif

#endif
