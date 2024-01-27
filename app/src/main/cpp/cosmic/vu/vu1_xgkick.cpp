#include <vu/vecu.h>
#include <gs/gif_bridge.h>
namespace cosmic::vu {
    void VectorUnit::issueXgKick() {
        updateDeltaCycles(clock.runCycles, true);
        for (; clock.runCycles > 0; clock.runCycles--) {
            updateMacPipeline();
            updateDivEfuPipes();
            intPipeline.update();
        }
        path1.stallXgKick = false;
    }
    void VectorUnit::startsKgKick2Gif() {
        if (!vu1Gif.has_value())
            return;
        (*vu1Gif)->reqADmacAtPath(1, true);
        while (path1.cycles >= 2) {
            if ((*vu1Gif)->isPathActivated(1, true))
        }

        path1.transferringGif = false;
    }
}