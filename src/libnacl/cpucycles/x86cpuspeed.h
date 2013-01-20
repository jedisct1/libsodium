/*
cpucycles x86cpuspeed.h version 20090716
Matthew Dempsky
Public domain.
*/

#ifndef CPUCYCLES_x86cpuspeed_h
#define CPUCYCLES_x86cpuspeed_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_x86cpuspeed(void);
extern long long cpucycles_x86cpuspeed_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "x86cpuspeed"
#define cpucycles cpucycles_x86cpuspeed
#define cpucycles_persecond cpucycles_x86cpuspeed_persecond
#endif

#endif
