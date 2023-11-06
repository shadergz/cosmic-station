#include <mio/dma_para.h>

namespace zenith::mio {
    DMAController::DMAController() {
        // I still don't know, perhaps the channels below don't have a FIFO list, so they will
        // always make direct requests without ordering
        channels[SprFrom].request = channels[SprTo].request = true;

        channels[IpuTo].request = true;
        channels[Sif1].request = true;

        channels[Vif0].request = channels[Vif1].request = true;
    }

    void DMAController::resetMA() {
        for (u8 dmIn{}; dmIn < 9; dmIn++) {
            channels[dmIn].index = dmIn;
        }
    }
    void DMAController::pulse(u32 cycles) {

    }
}
