#ifndef POLY1305_DONNA_H
#define POLY1305_DONNA_H

#include <stddef.h>

typedef struct poly1305_context {
        unsigned long long aligner;
        unsigned char opaque[136];
} poly1305_context;

#endif /* POLY1305_DONNA_H */

