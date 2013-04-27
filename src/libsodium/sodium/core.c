
#include <stdbool.h>

#include "core.h"
#include "crypto_onetimeauth.h"

static bool initialized;

int
sodium_init(void)
{
    if (initialized != 0) {
        return 1;
    }
    randombytes_stir();
    if (crypto_onetimeauth_pick_best_implementation() == NULL) {
        return -1;
    }
    initialized = 1;

    return 0;
}
