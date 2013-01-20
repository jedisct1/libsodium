/*
randombytes/devurandom.h version 20080713
D. J. Bernstein
Public domain.
*/

#ifndef randombytes_devurandom_H
#define randombytes_devurandom_H

#ifdef __cplusplus
extern "C" {
#endif

extern void randombytes(unsigned char *,unsigned long long);

#ifdef __cplusplus
}
#endif

#ifndef randombytes_implementation
#define randombytes_implementation "devurandom"
#endif

#endif
