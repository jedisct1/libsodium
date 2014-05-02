
#ifndef __SODIUM_RUNTIME_H__
#define __SODIUM_RUNTIME_H__ 1

#include "export.h"

SODIUM_EXPORT
int sodium_runtime_get_cpu_features(void);

SODIUM_EXPORT
int sodium_runtime_have_neon(void);

SODIUM_EXPORT
int sodium_runtime_have_sse2(void);

SODIUM_EXPORT
int sodium_runtime_have_sse3(void);

#endif
