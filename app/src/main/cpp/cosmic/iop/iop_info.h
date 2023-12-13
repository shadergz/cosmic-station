#pragma once
#include <common/types.h>
namespace cosmic::iop {
    class IoMipsCore;

    class IopExecVE {
    public:
        IopExecVE(raw_reference<IoMipsCore> mips)
            : ioMips(mips) {}

        virtual u32 executeCode() = 0;
        virtual u32 fetchPcInst() = 0;
        virtual ~IopExecVE() = default;
    protected:
        raw_reference<IoMipsCore> ioMips;
    };

    enum IOPSpecial {
        SpecialSyscall = 0xc,
        SpecialMfhi = 0x10,
        SpecialMthi = 0x11,
        SpecialOr = 0x25,
        SpecialNor = 0x27,
        SpecialXor = 0x26,
    };
    enum IOPCops {
        CopMfc = 0x0,
        CopMtc = 0x4,
        CopRfe = 0x10,
    };
    enum IOPOpcodes {
        SpecialOp = 0x0,
        Bne = 0x5,
        Blez = 0x6,
        Slti = 0x0a,
        Sltiu = 0x0b,
    };
}