
#include "core.h"
#include "crypto_onetimeauth.h"

static _Bool initialized;

int
sodium_init(void)
{
    if (initialized != 0) {
        return 1;
    }
    initialized = 1;
    if (crypto_onetimeauth_pick_best_implementation() == NULL) {
        return -1;
    }
    return 0;
}

int
sodium_reinit(void)
{
    return 0;
}

void
sodium_shutdown(void)
{
    
}
