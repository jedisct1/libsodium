/*
cpucycles sgi.h version 20070916
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_sgi_h
#define CPUCYCLES_sgi_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_sgi(void);
extern long long cpucycles_sgi_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "sgi"
#define cpucycles cpucycles_sgi
#define cpucycles_persecond cpucycles_sgi_persecond
#endif

#endif
