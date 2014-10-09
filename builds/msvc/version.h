
#ifndef __SODIUM_VERSION_H__
#define __SODIUM_VERSION_H__

#include "export.h"

#define SODIUM_VERSION_STRING "1.0.1"

#define SODIUM_LIBRARY_VERSION_MAJOR 7
#define SODIUM_LIBRARY_VERSION_MINOR 3

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
const char *sodium_version_string(void);

SODIUM_EXPORT
int         sodium_library_version_major(void);

SODIUM_EXPORT
int         sodium_library_version_minor(void);

#ifdef __cplusplus
}
#endif

#endif
