#include <vu/vecu.h>
namespace cosmic::vu {
    // We will halt the execution of instructions and advance the pipeline until the div/efu event concludes
    void VectorUnit::finishStallPipeTask(bool isDiv) {
        bool isWaiting;
        i64 after{isDiv ? status.div.finishAfter : status.efu.finishAfter};
        if (!isDiv)
            after -= 0x1;

        for ( ;; ) {
            isWaiting = clock.count < after;
            if (!isWaiting)
                break;
            // Simulating a wait, clock synchronization needs to be perfect here
            clock.count++;

            updateMacPipeline();
            intPipeline.update();
        }
        updateDivEfuPipes();
    }
}