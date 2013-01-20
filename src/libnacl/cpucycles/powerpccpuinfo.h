/*
cpucycles powerpccpuinfo.h version 20100803
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_powerpccpuinfo_h
#define CPUCYCLES_powerpccpuinfo_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_powerpccpuinfo(void);
extern long long cpucycles_powerpccpuinfo_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "powerpccpuinfo"
#define cpucycles cpucycles_powerpccpuinfo
#define cpucycles_persecond cpucycles_powerpccpuinfo_persecond
#endif

#endif
