#ifndef implementations_H
#define implementations_H

#include "export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

int _crypto_pwhash_argon2_pick_best_implementation(void);

#ifdef __cplusplus
}
#endif

#endif
