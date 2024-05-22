#pragma once
#include <optional>

#include <os/neon_simd.h>
#include <mio/dma_ctrl.h>
namespace cosmic::gs {
    class GsEngine;

    enum TagDataFormat {
        Packed,
        RegList,
        Image2,
        Image3,
        Unrecognized
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
    enum PathsTr {
        Vu1,
        Vif1,
        Gif = 3
    };
    class GifBridge {
    public:
        GifBridge(std::shared_ptr<GsEngine>& gsRef);
        void resetGif();

        bool downloadGsData(os::vec& put);
        void resumeDmacPath();
        void requestDmac(PathsTr path, bool intPath3 = false);
        void deactivatePath(PathsTr path);
        bool isPathActivated(PathsTr path, bool intPath3 = false);
        bool feedPathWithData(PathsTr whatPath, os::vec data);

        void update(u32 cycles);
    private:
        void transfer2Gif(os::vec packet);
        void decodeGifTag(Ref<GifTag>& unpacked, u64 packet[2]);
        void uploadPackedData(Ref<GifTag>& dsTag, u64 packet[2]);
        void queueReset();
        u64 queueGetSize();
        os::vec queueConsume();
        u64 queueFreePos();

        bool maskedPath3();
        void flushDmacFifo();

        std::shared_ptr<GsEngine> gs;
        std::array<GifPath, 4> paths;
        std::array<TagDataFormat, 4> pathsFormat;

        std::shared_ptr<mio::DmaController> dmac;

        GifStatus status;
        u8 activatePath;
        f32 gsQ;
        u8 pathQueue;

        u64 fifoSize;
        alignas(16) std::array<os::vec, 16> gifFifo;
        using QueueIterator = std::array<os::vec, 16>::iterator;
        static_assert(sizeof(gifFifo) == 256);

        QueueIterator fifoFront;
        std::array<bool, 16> fifoArr;

        u64 primitiveCounts;
        template <typename T>
        [[gnu::always_inline]] auto extractPair(u64 v, T a, u64 clean) {
            return static_cast<T>((v >> a) & clean);
        }
    };
}