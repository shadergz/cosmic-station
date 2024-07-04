#pragma once

#include <common/types.h>
namespace cosmic::ee {
    class EeMipsCore;
    class EeExecutor {
    public:
        EeExecutor(Ref<EeMipsCore>& mips) :
            eeCpu(mips) {}
        virtual u32 executeCode() = 0;
        virtual u32 fetchPcInst(u32 pc) = 0;
        virtual void performInvalidation(u32 address) = 0;
        virtual ~EeExecutor() = default;
    protected:
        Ref<EeMipsCore> eeCpu;
    };

    enum MipsRegsHw : u8 {
        $zero, // Hardwired 0 into the semiconductor
        $at,
        $v0, $v1, // MIPS compilers use these registers to return values from subroutines
        $a0, $a1, $a2, $a3, // And, of course, function parameter values - the rest are passed by the stack (nothing special)
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
        SpecialSra,
        SpecialSllv,
        SpecialSrlv = 0x6,
        SpecialSrav,
        // Note: This is our first flavor of JUMP added (Jump Register)
        SpecialJr,
        SpecialJalr,
        SpecialMovZ = 0xa,
        SpecialMovN,
        SpecialSyscall,
        SpecialBreak,
        SpecialSync = 0xf,
        SpecialMult = 0x18,
        SpecialMultu,
        SpecialDiv = 0x1a,
        SpecialDivu,
        SpecialAdd = 0x20,
        SpecialAddu,
        SpecialSub,
        SpecialSubu,
        SpecialAnd,
        SpecialOr,
        SpecialXor = 0x26,
        SpecialNor,

        SpecialDAdd = 0x2c,
        SpecialDAddu,
        SpecialDSub,
        SpecialDSubu,
        SpecialSlt,
    };
    enum MipsIvCopOp2 {
        CopOp2Tlbr = 0x1,
        CopOp2Tlbwi,
        CopOp2Eret = 0x18,
        CopOp2Ei = 0x38,
        CopOp2Di = 0x39,
    };
    enum MipsIvCops {
        Cop0Mfc = 0x0,

        Cop0Mtc = 0x4,
        Cop0Bc0 = 0x8,

        CopOp2Opcodes = 0x10,
        CopFpuOpcodes = 0x110
    };
    enum MipsRegImmOpcodes {
        RegImmBltzal = 0x10,
    };
    enum MipsIvOpcodes : u16 {
        SpecialOpcodes = 0x0,
        RegImmOpcodes,
        Bne = 0x5,
        Addi = 0x8,
        Addiu,
        Slti = 0xa,
        Sltiu,
        Andi,
        Ori,
        Xori,
        Lui,
        CopOpcodes = 0x10,
        Lb = 0x20,
        Lh,
        Lw = 0x23,
        Lbu,
        Lhu,
        Lwu = 0x27,
        Sw = 0x2b,
        Cache = 0x2f,
        Nop = 0x33,
        Ld = 0x37,
    };
    extern const std::array<std::string, 32> eeAllGprIdentifier;
}
