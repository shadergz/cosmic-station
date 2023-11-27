#include <common/types.h>
namespace cosmic::iop {
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
        Slti = 0x0a,
        Sltiu = 0x0b,
    };
}