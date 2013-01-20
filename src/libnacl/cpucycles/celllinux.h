/*
cpucycles celllinux.h version 20081201
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_celllinux_h
#define CPUCYCLES_celllinux_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_celllinux(void);
extern long long cpucycles_celllinux_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "celllinux"
#define cpucycles cpucycles_celllinux
#define cpucycles_persecond cpucycles_celllinux_persecond
#endif

#endif
