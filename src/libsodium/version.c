
#include "version.h"

const char *
sodium_version_string(void)
{
    return SODIUM_VERSION_STRING;
}

int
sodium_version_major(void)
{
    return SODIUM_VERSION_MAJOR;
}

int
sodium_version_minor(void)
{
    return SODIUM_VERSION_MINOR;
}
