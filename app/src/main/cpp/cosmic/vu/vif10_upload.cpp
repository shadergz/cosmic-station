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
    }

    void VifMalice::resetVif() {
        vifS.command = CmdIdle;
        vifS.isStalledVss = false;
        vifS.isStalledVfs = false;
        vifS.isStalledIntVis = false;
        vifS.interrupt = false;

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