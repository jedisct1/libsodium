/*
cpucycles amd64cpuspeed.h version 20090716
Matthew Dempsky
Public domain.
*/

#ifndef CPUCYCLES_amd64cpuspeed_h
#define CPUCYCLES_amd64cpuspeed_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_amd64cpuspeed(void);
extern long long cpucycles_amd64cpuspeed_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "amd64cpuspeed"
#define cpucycles cpucycles_amd64cpuspeed
#define cpucycles_persecond cpucycles_amd64cpuspeed_persecond
#endif

#endif
