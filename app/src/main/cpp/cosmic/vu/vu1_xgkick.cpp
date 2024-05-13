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
        quad = *BitCast<os::vec*>(&vecRegion.rw[addr]);
        if (vu1Gif.value()->feedPathWithData(1, quad)) {
            if (!path1.stallXgKick) {
                // Reactivating the previous interrupted transfer
                path1.stallXgKick = {};
                gifAddr = gifStallAddr;
                vu1Gif.value()->requestDmac(1, true);
            } else {
                vu1Gif.value()->deactivatePath(1);
                path1.transferringGif = {};
                return;
            }
        }
    }

    void VectorUnit::startXgKick2Gif() {
        if (!vu1Gif.has_value())
            return;
        vu1Gif.value()->requestDmac(1, true);
        while (path1.cycles >= 0x2) {
            if (!vu1Gif.value()->isPathActivated(1, true)) {
                path1.cycles = 0;
                break;
            }
            handleDataTransfer();
            path1.cycles -= 0x2;
        }
    }
}