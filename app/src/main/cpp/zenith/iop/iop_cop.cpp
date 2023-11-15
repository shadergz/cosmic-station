#include <iop/iop_cop.h>
#include <bitset>

namespace zenith::iop {
    static const u8 mask{0x1f};
    u32 IopCop::mfc(u8 copId) {
        u32 mcVar{};
        switch (copId) {
        case 12: {
            std::bitset<8*4> bin{};
            bin[0] = status.iec;
            bin[1] = status.kuc;
            bin[2] = status.iep;
            bin[4] = status.kup;
            bin[3] = status.ieo;
            bin[5] = status.kuo;
            bin[16] = status.isC;
            bin[22] = status.bev;

            mcVar |= CastU32(status.imm << 8);
            mcVar |= bin.to_ulong();
        }
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
        std::bitset<8*8> leaf{status.to64()};

        if (copId < 12)
            return ;
        leaf[0] = regV & 1;
        leaf[1] = regV & (1 << 1);
        leaf[2] = regV & (1 << 2);
        leaf[3] = regV & (1 << 3);
        leaf[4] = regV & (1 << 4);
        leaf[5] = regV & (1 << 5);
        leaf[6] = (regV >> 8) & 0xff;
        leaf[7] = regV & (1 << 16);
        status.bev = regV & (1 << 22);

        status.st64(leaf.to_ulong());
    }

    void IopCop::resetIOCop() {
        status.bev = true;
        cause.intPending = 0;
    }
}
