#include "crypto_core.h"

const char *primitiveimplementation = crypto_core_IMPLEMENTATION;
const char *implementationversion = crypto_core_VERSION;
const char *sizenames[] = { "outputbytes", "inputbytes", "keybytes", "constbytes", 0 };
const long long sizes[] = { crypto_core_OUTPUTBYTES, crypto_core_INPUTBYTES, crypto_core_KEYBYTES, crypto_core_CONSTBYTES };

void preallocate(void)
{
}

void allocate(void)
{
}

void measure(void)
{
}
