/*
   REDMASK51 is from amd64-51/consts.s.
*/

#ifdef __linux__
# error This is a work in progress, which is known *NOT* to work on Linux
# error Please do not fill a bug report at this point
#endif

#include "consts_namespace.h"
.data
.globl v0_0
.globl v1_0
.globl v2_1
.globl v2_2
.globl v9_0
.globl v9_9
.globl v19_19
.globl v38_1
.globl v38_19
.globl v38_38
.globl v121666_121666
.globl m25
.globl m26
.globl subc0
.globl subc2
.globl REDMASK51
.p2align 4
v0_0: .quad 0, 0
v1_0: .quad 1, 0
v2_1: .quad 2, 1
v2_2: .quad 2, 2
v9_0: .quad 9, 0
v9_9: .quad 9, 9
v19_19: .quad 19, 19
v38_1: .quad 38, 1
v38_19: .quad 38, 19
v38_38: .quad 38, 38
v121666_121666: .quad 121666, 121666
m25: .quad 33554431, 33554431
m26: .quad 67108863, 67108863
subc0: .quad 0x07FFFFDA, 0x03FFFFFE
subc2: .quad 0x07FFFFFE, 0x03FFFFFE
REDMASK51:   .quad 0x0007FFFFFFFFFFFF
