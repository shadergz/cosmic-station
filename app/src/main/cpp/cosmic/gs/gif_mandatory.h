#pragma once

#include <optional>
#include <os/neon_simd.h>
namespace cosmic::gs {
    class GsEngine;

    enum TagDataFormat {
        Packed,
        RegList,
        Image2,
        Image3
    };
    struct GifTag {
        u16 perLoop; // Also known as NLOOP
        bool isEndOfPacket;
        bool enbPrim;
        u16 sendToGsMyPrim; // Data to be sent to GS PRIM register if enbPrim is true
        TagDataFormat dataFormat;
        u8 regsNum;
    };
    struct GifPath {
        u8 status;
        GifTag tag;
    };

    struct GifStatus {
        bool path3enbGif;
        bool path3enbVifMask;
    };
    enum PathStatus {
        Busy = 0,
        Available = 4
    };

    // PATH1: VU1 via XGKICK instruction; Highest priority
    // PATH2: VIF1 via DIRECT/DIRECTHL; Medium priority
    // PATH3: GIF DMAC channel (channel 2); Lowest priority
    class GifArk {
    public:
        GifArk() = default;
        void resetGif();

        bool downloadGsData(os::vec& put);
        void resumeDmacPath();
        void reqADmacAtPath(u8 path, bool intPath3 = false);
    private:
        RawReference<GsEngine> gs;
        std::array<GifPath, 4> paths;

        GifStatus status;
        u8 activatePath;
    };
}