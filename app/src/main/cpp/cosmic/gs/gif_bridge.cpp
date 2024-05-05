#include <gs/gif_bridge.h>
#include <gs/synth_engine.h>
namespace cosmic::gs {
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
        if (!queueGetSize())
            return;
        paths[3].status = Busy;
        requestDmac(3);
    }
    void GifBridge::requestDmac(u8 path, bool intPath3) {
        if (!activatePath || activatePath == path) {
            activatePath = path;
            if (activatePath == 3 && (!maskedPath3() ||
                queueGetSize() <= 15)) {
            }
        } else {
            pathQueue |= 1 << path;
        }
        if (intPath3) {
        }
    }

    void GifBridge::resetGif() {
        for (u8 path{}; path < 3; path++) {
            paths[path].tag = GifTag{};
            paths[path].status = Available;
        }
        activatePath = 0;
        status = {};
        pathQueue = 0;
        gsQ = 0.f;

        queueReset();
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
        std::function<void(os::vec&)> feedDev;
        switch (path) {
        case 1:
            feedDev = [&](os::vec& graphics) {
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
            case Image3FmtTag:
                for (u8 pack{}; pack < 2; pack++)
                    gs->gsWrite(0x54, package[pack]);
                activated->leftRegsData[1]--;
                break;
            case Unrecognized:
                break;
            }
        }
    }
    void GifBridge::decodeGifTag(Ref<GifTag>& unpacked, u64 packet[2]) {
        unpacked->dataFormat = static_cast<TagDataFormat>(packet[0] >> 58 & 0x3);
        [[unlikely]] if (unpacked->dataFormat > Image3FmtTag) {
        }

        // The first transfer from Vif to GS is its Gif-Tag; let's decode it now
        unpacked->perLoop = packet[0] & 0x7fff;
        unpacked->isEndOfPacket = packet[0] & 1 << 0xf;
        unpacked->regs = packet[1];
        const u16 regs = packet[0] >> 60;

        if (!regs) {
            unpacked->regsNum = 0x10;
        }
        unpacked->leftRegsData[0] = unpacked->regsNum;
        unpacked->leftRegsData[1] = unpacked->perLoop;
    }
    void GifBridge::deactivatePath(u8 path) {
    }
    bool GifBridge::maskedPath3() {
        bool isMasked{};
        if (status.path3enbVifMask || status.path3enbGif) {
            isMasked = (pathsFormat[3] == TagDataFormat::Unrecognized);
            if (isMasked) {
                deactivatePath(3);
            }
        }
        return isMasked;
    }
}
