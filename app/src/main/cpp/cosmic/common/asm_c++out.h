#pragma once

#if __ASSEMBLER__
.macro declFunc func
    .type func, @function
.endm

.macro prologue alloc=16
    stp x29, x30, [sp, #-\alloc]!
.endm
.macro epilogue alloc=16
    ldp x29, x30, [sp], #\alloc
.endm

// Straight from the Linux kernel: https://github.com/torvalds/linux/blob/master/arch/arm64/include/asm/assembler.h
.macro adr_l, dst, sym
adrp \dst, \sym
add \dst, \dst, :lo12:\sym
.endm

.macro ldr_l, dst, sym, tmp=
.ifb \tmp
adrp \dst, \sym
    ldr \dst, [\dst, :lo12:\sym]
.else
adrp \tmp, \sym
    ldr \dst, [\tmp, :lo12:\sym]
.endif
.endm

.macro str_l, src, sym, tmp
adrp \tmp, \sym
    str \src, [\tmp, :lo12:\sym]
.endm
#endif
