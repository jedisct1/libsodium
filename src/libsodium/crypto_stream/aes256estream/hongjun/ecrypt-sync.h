
#ifndef __ECRYPT_SYNC__
#define __ECRYPT_SYNC__

#include <stdint.h>

typedef uint8_t  u8;
typedef uint32_t u32;

#define Nr 14
#define Nk 8
#define Nb 4

#pragma pack(push, 1)
__attribute__((aligned(64))) typedef struct ECRYPT_ctx
{
  u32 round_key[Nr+1][4];
  u32 counter[4];
  u32 first_round_output_x0;
  u32 second_round_output[4];
} ECRYPT_ctx;
#pragma pack(pop)

#define ECRYPT_encrypt_bytes(ctx, plaintext, ciphertext, msglen)   \
  ECRYPT_process_bytes(0, ctx, plaintext, ciphertext, msglen)

#endif
