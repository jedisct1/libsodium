/*
cpucycles sparccpuinfo.h version 20100803
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_sparccpuinfo_h
#define CPUCYCLES_sparccpuinfo_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_sparccpuinfo(void);
extern long long cpucycles_sparccpuinfo_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "sparccpuinfo"
#define cpucycles cpucycles_sparccpuinfo
#define cpucycles_persecond cpucycles_sparccpuinfo_persecond
#endif

#endif
