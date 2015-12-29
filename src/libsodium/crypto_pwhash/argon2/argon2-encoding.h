#ifndef argon2_encoding_H
#define argon2_encoding_H

#include "argon2.h"

int encode_string(char *dst, size_t dst_len, argon2_context *ctx,
                  argon2_type type);

int decode_string(argon2_context *ctx, const char *str, argon2_type type);

#endif
