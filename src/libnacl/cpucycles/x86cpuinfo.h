/*
cpucycles x86cpuinfo.h version 20100803
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_x86cpuinfo_h
#define CPUCYCLES_x86cpuinfo_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_x86cpuinfo(void);
extern long long cpucycles_x86cpuinfo_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "x86cpuinfo"
#define cpucycles cpucycles_x86cpuinfo
#define cpucycles_persecond cpucycles_x86cpuinfo_persecond
#endif

#endif
