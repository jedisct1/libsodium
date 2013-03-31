
#ifndef __SODIUM_VERSION_H__
#define __SODIUM_VERSION_H__

#define _STR(x) #x
#define STR(x) _STR(x)

#define SODIUM_VERSION_MAJOR 0
#define SODIUM_VERSION_MINOR 4

#define SODIUM_VERSION_STRING STR(SODIUM_VERSION_MAJOR) "." STR(SODIUM_VERSION_MINOR)

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT const char *sodium_version_string(void);
SODIUM_EXPORT int         sodium_version_major(void);
SODIUM_EXPORT int         sodium_version_minor(void);

#ifdef __cplusplus
}
#endif

#endif
