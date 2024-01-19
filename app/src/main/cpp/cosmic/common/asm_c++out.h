#pragma once

#define PROLOGUE_ASM(alloc)\
    __asm volatile("stp x29, x30, [sp, #-" #alloc "]!\n")
#define EPILOGUE_ASM(free)\
    __asm volatile("ldp x29, x30, [sp], #" #free "\n")

// Straight from the Linux kernel: https://github.com/torvalds/linux/blob/master/arch/arm64/include/asm/assembler.h
#define ADR_LTO_A64(dst, sym)\
    __asm volatile(          \
    "adrp "#dst", "#sym"\n"  \
    "add "#dst", "#dst", :lo12:" #sym)

/*
#define LDR_LTO_A64(dst, sym, tmp)\
    __asm volatile(               \
    "adrp "#tmp", "#sym"\n"       \
    "ldr "#dst", ["#tmp", :lo12:" #sym])
*/

#define STR_LTO_A64(src, sym, tmp)\
    __asm volatile(               \
    "adrp "#tmp", "#sym"\n"           \
    "str "#src", ["#tmp", :lo12:" #sym"]\n")
