
#ifndef __SODIUM_VERSION_H__
#define __SODIUM_VERSION_H__

#include "export.h"

#define SODIUM_VERSION_STRING "0.4.5"

#define SODIUM_LIBRARY_VERSION_MAJOR 4
#define SODIUM_LIBRARY_VERSION_MINOR 5

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
