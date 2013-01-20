/*
cpucycles dev4ns.h version 20100803
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_dev4ns_h
#define CPUCYCLES_dev4ns_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_dev4ns(void);
extern long long cpucycles_dev4ns_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "dev4ns"
#define cpucycles cpucycles_dev4ns
#define cpucycles_persecond cpucycles_dev4ns_persecond
#endif

#endif
