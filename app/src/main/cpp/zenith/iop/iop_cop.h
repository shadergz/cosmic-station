#pragma once
#include <common/types.h>
#define GetLane(cop, lane, pos) static_cast<u32>(vget_lane_u8(cop, lane) << pos)
#define Perform12(cop)\
    GetLane(copSet, 0, 0) |\
    GetLane(copSet, 1, 1) |\
    GetLane(copSet, 2, 2) |\
    GetLane(copSet, 3, 3) |\
    GetLane(copSet, 4, 4) |\
    GetLane(copSet, 5, 5) |\
    GetLane(copSet, 5, 8) |\
    GetLane(copSet, 7, 16)

#define CastU32(val) static_cast<u32>(val)
namespace zenith::iop {
    struct alignas(8) IopCopStatus {
        bool iec,
            kuc, iep, kup, ieo, kuo;
        u8 imm;
        bool isC,
            // `bev` needs to be set externally, as its value is not within the range of 8 bytes
            bev;
        auto to88() {
            return vld1_u8(as88());
        }
        u8* as88() {
            return bit_cast<u8*>(this);
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
        // Just for arithmetic reasons
        u88 copSet;

        u32 mfc(u8 copId);
        void mtc(u8 copId, u32 regV);
        void resetIOCop();
    };
}