#include <iop/iop_cop.h>

namespace zenith::iop {
    static const u8 mask{0x1f};
    u32 IopCop::mfc(u8 copId) {
        u32 mcVar{};
        switch (copId) {
        case 12:
            copSet = status.to88();
            mcVar = Perform12(copSet);
            break;
        case 13:
            mcVar |= CastU32(cause.code << 2);
            mcVar |= CastU32(cause.intPending << 8);
            mcVar |= CastU32(cause.bd << 31);
            break;
        case 14:
            mcVar = ePC;
            break;
        case 15:
            return 0x1f;
        }
        return mcVar;
    }
    void IopCop::mtc(u8 copId, u32 regV) {
        const u8 leafs[3]{
            static_cast<u8>(regV & 0x1f),
            static_cast<u8>((regV >> 8) & 0xff),
            static_cast<u8>((regV >> 16) & 0xff)};

        if (copId < 12)
            return ;
        copSet = vset_lane_u8(leafs[0] & 1, copSet, 0);
        copSet = vset_lane_u8(leafs[0] & (1 << 1), copSet, 1);
        copSet = vset_lane_u8(leafs[0] & (1 << 2), copSet, 2);
        copSet = vset_lane_u8(leafs[0] & (1 << 3), copSet, 3);
        copSet = vset_lane_u8(leafs[0] & (1 << 4), copSet, 4);
        copSet = vset_lane_u8(leafs[0] & (1 << 5), copSet, 5);
        copSet = vset_lane_u8(leafs[2] & 1, copSet, 7);

        vst1_u8(status.as88(), copSet);
        status.bev = leafs[2] & (1 << 6);
    }

    void IopCop::resetIOCop() {
        status.bev = true;
        cause.intPending = 0;
    }
}
