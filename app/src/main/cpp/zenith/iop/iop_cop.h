#pragma once
#include <common/types.h>

#define CastU32(val) static_cast<u32>(val)
namespace zenith::iop {
    struct alignas(8) IopCopStatus {
        bool iec,
            kuc, iep, kup, ieo, kuo;
        u8 imm;
        bool isC,
            // `bev` needs to be set externally, as its value is not within the range of 8 bytes
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
        void resetIOCop();
    };
}