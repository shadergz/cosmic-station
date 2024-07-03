#pragma once
#include <common/types.h>

namespace cosmic::iop {
    // https://psx-spx.consoledev.net/cpuspecifications/
    struct alignas(8) IopCopStatus {
        // iec: Current Interrupt Enable
        bool iec,
        // kuc: Current Kernel/User Mode (0=Kernel, 1=User)
            kuc,
        // iep: Previous Interrupt Disable
            iep,
        // kup: Previous Kernel/User Mode
            kup,
        // ieo: Old Interrupt Disable
            ieo,
        // kuo Old Kernel/User Mode
            kuo;
        u8 imm;
        bool isC,
            // bev: Boot exception vectors in RAM/ROM (0=RAM/KSEG0, 1=ROM/KSEG1)
            bev;
        // Just for arithmetic reasons
        auto to64() {
            return *BitCast<u64*>(this);
        }
        void st64(u64 value) {
            *BitCast<u64*>(this) = value;
        }
    };
    struct IopCopCause {
        u8 code;
        u8 intPending;
        bool bd;
    };

    class IopCop {
    public:
        IopCopStatus status;
        IopCopCause cause;
        u32 ePC;
        u32 c0id;

        u32 mfc(u8 copId) const;
        void mtc(u8 copId, u32 regVal);
        // Return from exception (COP0)
        void rfe();

        void resetIoCop();
    };
}