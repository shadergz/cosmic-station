#pragma once

#include <optional>
#include <os/neon_simd.h>
namespace cosmic::gs {
    class GsEngine;

    enum TagDataFormat {
        PackedFmtTag,
        RegListFmtTag,
        Image2FmtTag,
        Image3FmtTag
    };

    struct GifTag {
        u16 perLoop; // Also known as NLOOP
        bool isEndOfPacket;
        bool enbPrim;
        u16 sendToGsMyPrim; // Data to be sent to GS PRIM register if enbPrim is true
        TagDataFormat dataFormat;
        u8 regsNum;
        u64 regs;

        std::array<u32, 2> leftRegsData;
        bool isCompleted() {
            return !leftRegsData[1] && isEndOfPacket;
        }
    };
    struct GifPath {
        u8 status;
        GifTag tag;
    };

    struct GifStatus {
        bool path3enbGif;
        bool path3enbVifMask;
        bool tempStop;
    };
    enum PathStatus {
        Busy = 0,
        Available = 4
    };

    // PATH1: VU1 via XGKICK instruction; Highest priority
    // PATH2: VIF1 via DIRECT/DIRECTHL; Medium priority
    // PATH3: GIF DMAC channel (channel 2); Lowest priority
    class GifBridge {
    public:
        GifBridge() = default;
        void resetGif();

        bool downloadGsData(os::vec& put);
        void resumeDmacPath();
        void reqADmacAtPath(u8 path, bool intPath3 = false);
        bool isPathActivated(u8 path, bool intPath3 = false);
        bool feedPathWithData(u8 path, os::vec data);
    private:
        void transfer2Gif(os::vec packet);
        void decodeGifTag(Ref<GifTag>& t2dec, u64 packet[2]);
        void uploadPackedData(Ref<GifTag>& dsTag, u64 packet[2]);

        Ref<GsEngine> gs;
        std::array<GifPath, 4> paths;

        GifStatus status;
        u8 activatePath;
        f32 gsQ;

        u64 primitiveCounts;
        [[clang::always_inline]] u8 colorUnzip(u64 v, u8 a) {
            return static_cast<u8>((v << a) & 0xff);
        }
    };
}