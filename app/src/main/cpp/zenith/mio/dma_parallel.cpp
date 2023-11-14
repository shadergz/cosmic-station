#include <mio/dma_parallel.h>

namespace zenith::mio {
    DMAController::DMAController() {
        // I still don't know, perhaps the channels below don't have a FIFO list, so they will
        // always make direct requests without ordering
        channels[SprFrom].request = true;
        channels[SprTo].request = true;

        channels[IpuTo].request = true;
        channels[Sif1].request = true;

        channels[Vif0].request = true;
        channels[Vif1].request = true;
    }

    void DMAController::resetMA() {
        for (u8 dmIn{}; dmIn < 9; dmIn++) {
            channels[dmIn].index = dmIn;
        }
        priorityCtrl = 0;
    }
    void DMAController::pulse(u32 cycles) {}
    u32 DMAController::performRead(u32 address) {
        u32 request{};
        switch (address) {
        case 0x1000E010:
            request = intStatus;
            break;
        case 0x1000e020:
            request = *priorityCtrl;
            break;
        }
        return request;
    }
}
