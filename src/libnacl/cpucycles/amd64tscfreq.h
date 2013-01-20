/*
cpucycles amd64tscfreq.h version 20060318
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_amd64tscfreq_h
#define CPUCYCLES_amd64tscfreq_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_amd64tscfreq(void);
extern long long cpucycles_amd64tscfreq_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "amd64tscfreq"
#define cpucycles cpucycles_amd64tscfreq
#define cpucycles_persecond cpucycles_amd64tscfreq_persecond
#endif

#endif
