#ifdef HAVE_AVX_ASM

#define IN_SANDY2X

#include "consts.S"
#include "fe51_mul.S"
#include "fe51_pack.S"
#include "ladder.S"

#endif

#if defined(__ELF__) && \
    (defined(__linux__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || \
     defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__) || \
     defined(__gnu_hurd__))
.section .note.GNU-stack,"",%progbits
#endif
