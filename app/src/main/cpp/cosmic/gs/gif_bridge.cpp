#include <gs/gif_bridge.h>
#include <gs/synth_engine.h>

namespace cosmic::gs {
    void gifQueueReset();
    u8 gifQueueSize();

    bool GifBridge::downloadGsData(os::vec& put) {
        auto gsResult{gs->readGsData()};
        if (std::get<0>(gsResult))
            put = std::get<1>(gsResult);
        return std::get<0>(gsResult);
    }
    void GifBridge::resumeDmacPath() {
        if (status.path3enbGif || status.path3enbVifMask)
            return;
        if (paths[3].status != Available)
            return;
        if (!gifQueueSize())
            return;
        paths[3].status = Busy;
        reqADmacAtPath(0x3);
    }
    void GifBridge::reqADmacAtPath(u8 path, bool intPath3) {
    }

    void GifBridge::resetGif() {
        for (u8 path{}; path < 3; path++) {
            paths[path].tag = GifTag{};
            paths[path].status = Available;
        }
        activatePath = 0;
        status = {};
        gsQ = 0.f;

        gifQueueReset();
    }
    bool GifBridge::isPathActivated(u8 path, bool intPath3) {
        if (path != 3 && intPath3) {
        }
        const bool isSelected{activatePath == path};
        return isSelected &&
            !gs->isStalled() &&
            !status.tempStop &&
            !gs->privileged(GsBusDir);
    }
    bool GifBridge::feedPathWithData(u8 path, os::vec data) {
        std::function<void(os::vec)> feedDev;
        switch (path) {
        case 1:
            feedDev = [&](os::vec graphics) {
                transfer2Gif(graphics);
            };
            break;
        }
        if (feedDev)
            feedDev(data);
        return (path == 1) &&
            paths[path].tag.isCompleted();
    }
    void GifBridge::transfer2Gif(os::vec packet) {
        std::array<u64, 2> package{};
        for (u8 pack{}; pack < 2; pack++)
            package[pack] = packet.to64(pack);

        Ref<GifTag> activated{std::ref(paths[activatePath].tag)};
        if (!activated->leftRegsData[1]) {
            primitiveCounts++;

            decodeGifTag(activated, package.data());
            gsQ = 1.0;

            if (activated->leftRegsData[1] != 0) {

            }
        } else {
            switch (activated->dataFormat) {
            case PackedFmtTag:
                // This is an element loop count, like N * M, where N is the count of regs and M is
                // the number of times the regs data packet needs to be transferred
                activated->leftRegsData[0]--;
                uploadPackedData(activated, package.data());

                if (!activated->leftRegsData[0]) {
                    activated->leftRegsData[0] = activated->regsNum;
                    activated->leftRegsData[1]--;
                }
                break;
            case RegListFmtTag:
                break;
            case Image2FmtTag:
                break;
            case Image3FmtTag:
                break;
            }
        }
    }
    void GifBridge::decodeGifTag(Ref<GifTag>& t2dec, u64 packet[2]) {
        u16 lo, fmt, regs;
        bool end;

        lo = packet[0] & 0x7fff;
        end = packet[0] & 1 << 0xf;
        fmt = packet[0] >> 58 & 0x3;
        regs = packet[0] >> 60;
        [[unlikely]] if (fmt > Image3FmtTag)
            ;

        // The first transfer from Vif to GS is its Gif-Tag; let's decode it now
        t2dec->perLoop = lo;
        t2dec->isEndOfPacket = end;
        t2dec->dataFormat = static_cast<TagDataFormat>(fmt);

        if (!regs)
            t2dec->regsNum = 0x10;

        t2dec->leftRegsData[0] = t2dec->regsNum;
        t2dec->leftRegsData[1] = t2dec->perLoop;
    }
}
