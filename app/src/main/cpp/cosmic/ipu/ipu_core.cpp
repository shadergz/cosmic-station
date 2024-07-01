#include <ipu/ipu_core.h>

namespace cosmic::ipu {
    IpuMpeg2::IpuMpeg2(std::shared_ptr<mio::DmaController>& direct) :
        dmac(direct) {
        status.rst = true;
        u32 index;

        // Generates the CrCb->RGB conversion table in a pre-calculated table, just like DobieStation does
        for (u16 oui{}; oui < 0x40; oui += 8) {
            for (u16 oub{}; oub < 0x10; oub += 2) {
                index = oub + (oui * 4);
                u8 rgbMapIdx{static_cast<u8>((oub / 2) + oui)};

                crCbMap[index + 0] = rgbMapIdx;
                crCbMap[index + 1] = rgbMapIdx;
                crCbMap[index + 0x10] = rgbMapIdx;
                crCbMap[index + 0x11] = rgbMapIdx;
            }
        }
        ditherMtx.clear();
        ditherMtx[0][0] = 4;
        ditherMtx[0][2] = static_cast<u8>(-3);
        ditherMtx[0][3] = 1;
        ditherMtx[1][0] = 2;
        ditherMtx[1][1] = static_cast<u8>(-2);
        ditherMtx[1][2] = 3;
        ditherMtx[1][3] = static_cast<u8>(-1);
        ditherMtx[2][0] = static_cast<u8>(-3);
        ditherMtx[2][1] = 1;
        ditherMtx[2][2] = static_cast<u8>(-4);
        ditherMtx[3][0] = 3;
        ditherMtx[3][1] = static_cast<u8>(-1);
        ditherMtx[3][2] = 2;
        ditherMtx[3][3] = static_cast<u8>(-2);
    }

    void IpuMpeg2::resetDecoder() {
        status = {};
        status.pictureCode = PictureVDec::I;
        in.resetDeck();
        out.resetDeck();
    }
    void IpuMpeg2::update() {
        constexpr auto to{mio::DirectChannels::IpuTo};
        constexpr auto from{mio::DirectChannels::IpuFrom};

        if (status.isBusy) {
            bool proCmd{true};
            if (coreAction == 0x01 ||
                coreAction == 0x02 ||
                coreAction == 0x03 ||
                coreAction == 0x04) {
                if (!in.size)
                    proCmd = false;
            }
            if (proCmd) {
                switch (coreAction) {
                case 0x01:
                    if (processSliceDecode())
                        clearActionWithInt();
                    break;
                }
            }
        }

        if (fifoIsEchoing(FifoLayout::In))
            dmac->issueADmacRequest(to);
        if (fifoIsEchoing(FifoLayout::Out))
            dmac->issueADmacRequest(from);
    }
    bool IpuMpeg2::fifoIsEchoing(FifoLayout fifo) {
        // We could write data into the FIFO
        if (fifo == FifoLayout::In)
            return in.size < 8;
        // We could read data from the FIFO
        return out.size > 0;
    }

    void IpuMpeg2::issueACmd(u32 cmd) {
    }

    bool IpuMpeg2::processSliceDecode() {
        std::array<bool, 2> states{};
        do {
            switch (iDec.state) {
            case Qsc:
                states[0] = false;
                states[1] = true;
                break;
            case Done:
            default:
                ranges::fill(states, true);
                break;
            }
        } while (!states[1]);
        return states[0];
    }
    void IpuMpeg2::clearActionWithInt() {
        status.isBusy = {};
    }
    std::array<u32, 0x40> IpuMpeg2::inverseScanZz{
         0,  1,  5,  6, 14, 15, 27, 28,
         2,  4,  7, 13, 16, 26, 29, 42,
         3,  8, 12, 17, 25, 30, 41, 43,
         9, 11, 18, 24, 31, 40, 44, 53,
        10, 19, 23, 32, 39, 45, 52, 54,
        20, 22, 33, 38, 46, 51, 55, 60,
        21, 34, 37, 47, 50, 56, 59, 61,
        35, 36, 48, 49, 57, 58, 62, 63
    };
    std::array<u32, 0x40> IpuMpeg2::inverseScanAlternate{
         0,  4,  6, 20, 22, 36, 38, 52,
         1,  5,  7, 21, 23, 37, 39, 53,
         2,  8, 19, 24, 34, 40, 50, 54,
         3,  9, 18, 25, 35, 41, 51, 55,
        10, 17, 26, 30, 42, 46, 56, 60,
        11, 16, 27, 31, 43, 47, 57, 61,
        12, 15, 28, 32, 44, 48, 58, 62,
        13, 14, 29, 33, 45, 49, 59, 63
    };
    std::array<u32, 0x20> IpuMpeg2::quantizerLinear{
         0,  2,  4,  6,  8, 10, 12, 14,
        16, 18, 20, 22, 24, 26, 28, 30,
        32, 34, 36, 38, 40, 42, 44, 46,
        48, 50, 52, 54, 56, 58, 60, 62
    };
    std::array<u32, 0x20> IpuMpeg2::quantizerNonLinear{
         0,  1,  2,  3,  4,  5,   6,   7,
         8, 10, 12, 14, 16, 18,  20,  22,
        24, 28, 32, 36, 40, 44,  48,  52,
        56, 64, 72, 80, 88, 96, 104, 112,
    };
}
