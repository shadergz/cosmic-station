#include <vu/vif10_upload.h>
#include <vu/vecu.h>

namespace cosmic::vu {
    VifMalice::VifMalice(Ref<VectorUnit> vector, VifGifInterconnector card) :
            vif2gif(card), vifVu(vector) {

        tops[0] = {};
        tops[1] = {};
        vifVu->establishVif(tops, *vif2gif.gif.get());
    }
    void VifMalice::update(u32 cycles) {
        i64 expandable{cycles};
        if (fifoState == GifDownloading) {
            // Apparently, the VIF downloads data from GS through the Gif and
            // puts this data in its queue in reverse mode
            vifS.command = CmdIdle;
            while (expandable--) {
                if (fifo.size() > (fifoSize - 4))
                    break;
                os::vec packet{};
                if (!vif2gif.gif->downloadGsData(packet))
                    return;
                fifo.pushQuad(packet);
            }
            return;
        }
        // Following DobieStation steps, we can double the number of cycles and process 1
        // QuadWord (4 * 4) per bus cycle
        u32 doubledCycles{cycles << 2};
        if (isVifStalled & MskPath3) {
            if (!vif2gif.getId()) {
            }
            // Activates the third data path between the Gif and the DMAC
            vif2gif.gif->resumeDmacPath();
            isVifStalled &= ~MskPath3;
        }

        for ( ; isVifStalled == NotStalled && doubledCycles--; ) {
            while (fifoState == Cooking &&
                getQueueFreeSpace() < 8 && getFifoFreeSpace()) {
                inQueue.push(fifo.consume());
            }
        }
    }

    void VifMalice::resetVif() {
        vifS.command = CmdIdle;
        vifS.isStalledVss = false;
        vifS.isStalledVfs = false;
        vifS.isStalledIntVis = false;
        vifS.interrupt = false;

        vifS.vewWaitingVu = false;
        vifS.vgwWaitingGif = false;

        fifoState = Cooking;

        num = 0;
        mask = code = 0;
        // VU1 has 4 times more memory than VU0
        if (vif2gif.getId()) {
            memMask = 0x3ff;
            fifoSize = 64;
            vifId = mio::DirectChannels::Vif1;
        } else {
            memMask = 0xff;
            fifoSize = 32;
            vifId = mio::DirectChannels::Vif0;
        }

        fifo.resetVifFifo();
        inQueue.resetVifFifo();
    }
    bool VifMalice::transferDmaData(os::vec quad, bool validateFreeSpace) {
        if (validateFreeSpace) {
            if (getFifoFreeSpace() == 0) {
                dmac->disableChannel(vifId, true);
                return false;
            }
        }
        for ( u8 part{}; part < 4; part++) {
            fifo.push(quad.to32(part));
        }
        dmac->advanceSrcDma(vifId);
        return true;
    }
}