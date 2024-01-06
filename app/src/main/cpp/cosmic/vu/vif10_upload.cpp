#include <vu/vif10_upload.h>
#include <vu/vecu.h>

namespace cosmic::vu {
    VifMalice::VifMalice(RawReference<VectorUnit> vector, VifGifInterconnector card) :
        vif2gif(card), vifVU(vector) {

        tops[0] = {};
        tops[1] = {};
        vifVU->establishVif(tops, *vif2gif.gif.get());
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
        // Following DobieStation's steps, we can double the number of cycles and process 1
        // QuadWord (4 * 4) per Bus cycle
        u32 doubledCycles{cycles << 2};
        if (isVifStalled & MskPath3) {
            if (!vif2gif.getId())
                ;
            // Activates the third data path between the Gif and the DMAC
            vif2gif.gif->resumeDmacPath();
            isVifStalled &= ~MskPath3;
        }
        std::array<bool, 1> isLoopNeeded{};
        for ( ;; ) {
            isLoopNeeded[0] = isVifStalled == NotStalled && doubledCycles--;
            if (!isLoopNeeded[0])
                break;
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
        } else {
            memMask = 0xff;
            fifoSize = 32;
        }
        fifo = FifoStates(fifoSize);
    }
}