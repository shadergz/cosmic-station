#include <gs/gif_bridge.h>
#include <gs/gs_engine.h>
namespace cosmic::gs {
    GifBridge::GifBridge(std::shared_ptr<GsEngine>& gsRef) :
        gs(gsRef),
        dmac(gsRef->shared->controller) {

    }
    bool GifBridge::downloadGsData(os::vec& put) {
        const auto gsResult{gs->readGsData()};
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
        requestDmac(Gif);
    }
    void GifBridge::requestDmac(PathsTr path, bool intPath3) {
        if (!activatePath || activatePath == path) {
            activatePath = path;
            if (activatePath == 3 && (!maskedPath3() ||
                queueGetSize() <= 15)) {
                dmac->issueADmacRequest(mio::Gif);
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
    void GifBridge::flushDmacFifo() {
        feedPathWithData(Gif, queueConsume());
        if (!queueGetSize()) {
            if (maskedPath3()) {
            } else {
                requestDmac(Gif);
            }
        }
    }
    void GifBridge::update(u32 cycles) {
        if (!maskedPath3() && !queueGetSize()) {
            requestDmac(Gif);
        }
        while (isPathActivated(Gif) && !maskedPath3() &&
                cycles && queueGetSize()) {
            flushDmacFifo();
            cycles--;
        }
    }
    bool GifBridge::isPathActivated(PathsTr path, bool intPath3) {
        if (path != Gif && intPath3) {
        }
        const bool isSelected{activatePath == path};
        return isSelected &&
            !gs->isStalled() &&
            !status.tempStop &&
            !gs->privileged(GsBusDir);
    }
    bool GifBridge::feedPathWithData(PathsTr whatPath, os::vec data) {
        std::function<void(os::vec&)> feedDev;
        switch (whatPath) {
        case Vu1:
        case Vif1:
        case Gif:
            feedDev = [&](os::vec& graphics) {
                transfer2Gif(graphics);
            };
            break;
        }
        if (feedDev)
            feedDev(data);
        return (whatPath == 1) &&
            paths[whatPath].tag.isCompleted();
    }
    void GifBridge::transfer2Gif(const os::vec& packet) {
        std::array<u64, 2> package{};
        for (u8 pack{}; pack < 2; pack++)
            package[pack] = packet.to64(pack);

        auto& activated{paths[activatePath].tag};
        if (!activated.leftRegsData[1]) {
            primitiveCounts++;
            decodeGifTag(activated, package.data());
            // NOTE: The GS Q register is initialized to 1.0f when reading a GIFtag
            gsQ = 1.0;

            if (activated.leftRegsData[1] != 0) {
            }
        } else {
            switch (activated.dataFormat) {
            case TagDataFormat::Packed:
                // This is an element loop count, like N * M, where N is the count of regs and M is
                // the number of times the regs data packet needs to be transferred
                activated.leftRegsData[0]--;
                uploadPackedData(activated, package.data());

                if (!activated.leftRegsData[0]) {
                    activated.leftRegsData[0] = activated.regsNum;
                    activated.leftRegsData[1]--;
                }
                break;
            case TagDataFormat::RegList:
                break;
            case TagDataFormat::Image2:
            case TagDataFormat::Image3:
                for (u8 pack{}; pack < 2; pack++)
                    gs->gsWrite(0x54, package[pack]);
                activated.leftRegsData[1]--;
                break;
            case TagDataFormat::Unrecognized:
                break;
            }
        }
    }
    void GifBridge::decodeGifTag(GifTag& unpacked, u64 packet[2]) {
        unpacked.dataFormat = static_cast<TagDataFormat>(packet[0] >> 58 & 0x3);
        if (unpacked.dataFormat > TagDataFormat::Image3) {
        }
        // The first transfer from Vif to GS is its Gif-Tag let's decode it now
        unpacked.perLoop = packet[0] & 0x7fff;
        unpacked.isEndOfPacket = packet[0] & 1 << 0xf;
        unpacked.regs = packet[1];

        const u8 regs = packet[0] >> 60;
        unpacked.regsNum = regs;
        if (!regs) {
            unpacked.regsNum = 0x10;
        }
        unpacked.leftRegsData[0] = unpacked.regsNum;
        unpacked.leftRegsData[1] = unpacked.perLoop;
    }
    void GifBridge::deactivatePath(PathsTr path) {
    }
    bool GifBridge::maskedPath3() {
        bool isMasked{};
        if (status.path3enbVifMask || status.path3enbGif) {
            isMasked = (pathsFormat[3] == TagDataFormat::Unrecognized);
            if (isMasked) {
                deactivatePath(Gif);
            }
        }
        return isMasked;
    }
}
