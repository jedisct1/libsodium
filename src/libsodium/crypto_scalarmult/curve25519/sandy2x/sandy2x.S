
#ifdef HAVE_AVX_ASM

#define IN_SANDY2X

#include "consts.S"
#include "fe51_mul.S"
#include "fe51_nsquare.S"
#include "fe51_pack.S"
#include "ladder.S"

#if defined(__linux__) && defined(__ELF__)
#if defined(__CET__)
.section	.note.gnu.property,"a"
.p2align 3
.long	1f - 0f
.long	4f - 1f
.long	5
0:
.string	"GNU"
1:
.p2align 3
.long	0xc0000002
.long	3f - 2f
2:
.long	__CET__
3:
.p2align 3
4:
#endif
.section .note.GNU-stack,"",%progbits
#endif

#endif
