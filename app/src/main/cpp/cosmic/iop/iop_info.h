#pragma once
#include <common/types.h>
namespace cosmic::iop {
    class IoMipsCore;

    class IopExecVe {
    public:
        IopExecVe(raw_reference<IoMipsCore> mips) :
            ioMips(mips) {}

        virtual u32 executeCode() = 0;
        virtual u32 fetchPcInst() = 0;
        virtual ~IopExecVe() = default;
    protected:
        raw_reference<IoMipsCore> ioMips;
    };

    enum IopSpecial {
        SpecialSyscall = 0xc,
        SpecialMfhi = 0x10,
        SpecialMthi = 0x11,
        SpecialOr = 0x25,
        SpecialNor = 0x27,
        SpecialXor = 0x26,
    };
    enum IopCops {
        CopMfc = 0x000,
        CopMtc = 0x004,
        CopRfe = 0x010,
    };
    enum IopOpcodes {
        SpecialOp = 0x0,
        Bne = 0x5,
        Blez = 0x6,
        Slti = 0x0a,
        Sltiu = 0x0b,
    };
}