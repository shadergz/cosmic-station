#pragma once
#include <common/types.h>

#define CastU32(val) static_cast<u32>(val)
namespace zenith::iop {
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
            // `bev` needs to be set externally, as its value is not within the range of 8 bytes
            // bev: Boot exception vectors in RAM/ROM (0=RAM/KSEG0, 1=ROM/KSEG1)
            bev;
        // Just for arithmetic reasons
        auto to64() {
            return *bit_cast<u64*>(this);
        }
        void st64(u64 value) {
            *bit_cast<u64*>(this) = value;
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

        u32 mfc(u8 copId);
        void mtc(u8 copId, u32 regV);
        // Return from exception (COP0)
        void rfe();

        void resetIOCop();
    };
}