#include <mio/dma_parallel.h>

namespace cosmic::mio {
    DmaController::DmaController() {
        // I still don't know, perhaps the channels below don't have a FIFO list, so they will
        // always make direct requests without ordering
        channels[SprFrom].request = true;
        channels[SprTo].request = true;

        channels[IpuTo].request = true;
        channels[Sif1].request = true;

        channels[Vif0].request = true;
        channels[Vif1].request = true;
    }

    void DmaController::resetMA() {
        for (u8 dmIn{}; dmIn < 9; dmIn++) {
            channels[dmIn].index = dmIn;
            channels[dmIn].request = false;
        }
        priorityCtrl = 0;
    }
    void DmaController::pulse(u32 cycles) {}
    os::vec128 DmaController::performRead(u32 address) {
        os::vec128 request{};
        switch (address) {
        case 0x1000e010:
            request = intStatus;
            break;
        case 0x1000e020:
            request = *priorityCtrl;
            break;
        }
        return request;
    }
    void DmaController::issueADmacRequest(DirectChannels channel) {
        channels[channel].request = true;
    }
}
