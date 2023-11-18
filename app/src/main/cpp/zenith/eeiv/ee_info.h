#pragma once

namespace zenith::eeiv {
    enum MipsRegsHw : u8 {
        $zero, // Hardwired 0 into the semiconductor
        $at,
        $v0, $v1, // MIPS compilers use these registers to return values from subroutines
        $a0, $a1, $a2, $a3, // And, of course, function parameter values; the rest are passed by the stack (nothing special)
        $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7,
        $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7,
        $t8, $t9,
        $k0, $k1,
        $gp, // Global pointer (I don't have any information of what this means for now)
        $sp, // Stack pointer
        $fp, // Frame pointer
        $ra, // Return address
    };
}

namespace zenith::fuji {
    enum MipsIVOpcodes : u16 {
        SpecialOpcodes = 0x0,
        Cop0Mfc = 0x0,
        RegImmOpcodes = 0x1,
        CopOp2Tlbr = 0x1,
        Cop0Mtc = 0x4,
        Cop0Bc0 = 0x8,
        Addi = 0x8,
        EeSlti = 0xa,
        EeSpecialSyscall = 0xc,
        SpecialBreak = 0xd,
        RegImmBltzal = 0x10,
        CopOpcodes = 0x10,
        CopOp2 = 0x10,
        CopOp2Eret = 0x18,
        CopOp2Ei = 0x38,
        CopOp2Di = 0x39,
        Lb = 0x20,
        Lh = 0x21,
        Lw = 0x23,
        Lbu = 0x24,
        Lhu = 0x25,
        Lwu = 0x27,
        Cache = 0x2f,
        Nop = 0x33,
        Ld = 0x37,

        EeSpecialXor = 0x26,
        SpecialSlt = 0x2a,
        Sw = 0x2b,
    };
}
