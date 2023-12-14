#pragma once

#include <common/types.h>
namespace cosmic::engine {
    class EeMipsCore;
    class EeExecutor {
    public:
        EeExecutor(raw_reference<EeMipsCore> mips) :
            mainMips(mips) {}
        virtual u32 executeCode() = 0;
        virtual u32 fetchPcInst(u32 pc) = 0;
        virtual void performInvalidation(u32 address) = 0;
        virtual ~EeExecutor() = default;
    protected:
        raw_reference<EeMipsCore> mainMips;
    };

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

    enum MipsIvSpecial : u16 {
        SpecialSll = 0x0,
        SpecialSrl = 0x2,
        SpecialSra = 0x3,
        SpecialSllv = 0x4,
        SpecialSrlv = 0x6,
        SpecialSrav = 0x7,
        SpecialMovZ = 0xa,
        SpecialMovN = 0xb,
        SpecialSyscall = 0xc,
        SpecialBreak = 0xd,
        SpecialMult = 0x18,
        SpecialMultu = 0x19,
        SpecialDiv = 0x1a,
        SpecialDivu = 0x1b,
        SpecialAdd = 0x20,
        SpecialAddu = 0x21,
        SpecialSub = 0x22,
        SpecialSubu = 0x23,
        SpecialDAdd = 0x2c,
        SpecialDAddu = 0x2d,
        SpecialDSub = 0x2e,
        SpecialDSubu = 0x2f,
        SpecialXor = 0x26,
        SpecialSlt = 0x2a,
    };
    enum MipsIvCops {
        Cop0Mfc = 0x0,
        CopOp2Tlbr = 0x1,

        Cop0Mtc = 0x4,
        Cop0Bc0 = 0x8,

        CopOp2Opcodes = 0x10,
        CopOp2Eret = 0x18,
        CopOp2Ei = 0x38,
        CopOp2Di = 0x39,
    };
    enum RegImmOpcodes {
        RegImmBltzal = 0x10,
    };
    enum MipsIvOpcodes : u16 {
        SpecialOpcodes = 0x0,
        RegImmOpcodes = 0x1,
        Bne = 0x5,
        Addi = 0x8,
        Slti = 0xa,
        CopOpcodes = 0x10,
        Lb = 0x20,
        Lh = 0x21,
        Lw = 0x23,
        Lbu = 0x24,
        Lhu = 0x25,
        Lwu = 0x27,
        Cache = 0x2f,
        Nop = 0x33,
        Ld = 0x37,
        Sw = 0x2b,
    };
    extern const std::array<const char*, 32> gprsId;
}
