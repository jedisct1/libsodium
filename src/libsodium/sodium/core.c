
#include "core.h"
#include "crypto_onetimeauth.h"

static char initialized;

int
sodium_init(void)
{
    if (__sync_lock_test_and_set(&initialized, 1) != 0) {
        return 1;
    }
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
