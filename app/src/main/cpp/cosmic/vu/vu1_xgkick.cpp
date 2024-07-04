#include <vu/vecu.h>
#include <gs/gif_bridge.h>
namespace cosmic::vu {
    void VectorUnit::stallByXgKick() {
        updateDeltaCycles(clock.runCycles, true);
        for (; clock.runCycles > 0; clock.runCycles--) {
            updateMacPipeline();
            updateDivEfuPipes();
            intPipeline.update();
        }
        path1.stallXgKick = false;
    }
    void VectorUnit::handleDataTransfer() {
        os::vec quad;
        u16 addr{gifAddr};

        gifAddr += 16;
        std::memcpy(&quad, &vecRegion.rw[addr], sizeof(quad));
        if (vu1Gif->feedPathWithData(gs::Vu1, quad)) {
            if (!path1.stallXgKick) {
                // Reactivating the previous interrupted transfer
                path1.stallXgKick = {};
                gifAddr = gifStallAddr;
                vu1Gif->requestDmac(gs::Vu1, true);
            } else {
                vu1Gif->deactivatePath(gs::Vu1);
                path1.transferringGif = {};
                return;
            }
        }
    }

    void VectorUnit::startXgKick2Gif() {
        if (!vu1Gif)
            return;
        vu1Gif->requestDmac(gs::Vu1, true);
        while (path1.cycles >= 0x2) {
            if (!vu1Gif->isPathActivated(gs::Vu1, true)) {
                path1.cycles = 0;
                break;
            }
            handleDataTransfer();
            path1.cycles -= 0x2;
        }
    }
}