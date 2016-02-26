#ifndef core_hchacha20_H
#define core_hchacha20_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define U8C(v)  (v##U)
#define U32C(v) (v##U)

#define U8V(v)  ((uint8_t)(v) & U8C(0xFF))
#define U32V(v) ((uint32_t)(v) & U32C(0xFFFFFFFF))

#define ROTL32(v, n) (U32V((v) << (n)) | ((v) >> (32 - (n))))

#define ROTATE(v, c) (ROTL32(v, c))
#define XOR(v, w)    ((v) ^ (w))
#define PLUS(v, w)   (U32V((v) + (w)))

#define QUARTERROUND(a, b, c, d) \
  do { \
      a = PLUS(a, b); d = ROTATE(XOR(d, a), 16); \
      c = PLUS(c, d); b = ROTATE(XOR(b, c), 12); \
      a = PLUS(a, b); d = ROTATE(XOR(d, a),  8); \
      c = PLUS(c, d); b = ROTATE(XOR(b, c),  7); \
  } while(0)

static inline uint32_t
load32(const void *src)
{
#ifdef NATIVE_LITTLE_ENDIAN
    uint32_t w;
    memcpy(&w, src, sizeof w);
    return w;
#else
    const uint8_t *p = (const uint8_t *) src;
    uint32_t w = *p++;
    w |= (uint32_t)(*p++) <<  8;
    w |= (uint32_t)(*p++) << 16;
    w |= (uint32_t)(*p++) << 24;
    return w;
#endif
}

static inline void
store32(void *dst, uint32_t w)
{
#ifdef NATIVE_LITTLE_ENDIAN
    memcpy(dst, &w, sizeof w);
#else
    uint8_t *p = (uint8_t *) dst;
    *p++ = (uint8_t) w; w >>= 8;
    *p++ = (uint8_t) w; w >>= 8;
    *p++ = (uint8_t) w; w >>= 8;
    *p++ = (uint8_t) w;
#endif
}

#endif
