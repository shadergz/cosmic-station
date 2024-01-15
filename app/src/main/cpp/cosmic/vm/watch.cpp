#include <common/global.h>
#include <vm/watch.h>
namespace cosmic::vm {
    WatchStatus::WatchStatus() {
        starts = finish = std::chrono::high_resolution_clock::now();
    }
    void WatchStatus::setDesiredFrames(u8 fps) {
        if (desiredFps != fps) {
            eeVu.cpuCycles = desiredFps * 4'000'000;
            iopBus.ioCycles = desiredFps * 2'000'000;

            eeVu.cpuAcc = 0;
            iopBus.ioCycles = 0;
        }
        desiredFps = fps;
    }
    void WatchStatus::checkStatus() {
        if (eeVu.cpuAcc >= eeVu.cpuCycles)
            markStepsDone();
        if (iopBus.ioAcc >= eeVu.cpuCycles)
            markStepsDone();

        if (isFrameCompleted)
            markStepsDone();
    }

    void WatchStatus::frameFinished(u64 eeCycles, u64 busCycles) {
        eeVu.cpuAcc += eeCycles;
        iopBus.ioAcc += busCycles;
        isFrameCompleted = true;
        finish = std::chrono::high_resolution_clock::now();
    }

    void WatchStatus::markStepsDone() {
        auto currentTime{std::chrono::high_resolution_clock::now()};
        auto elapsedMicro{
            std::chrono::duration_cast<std::chrono::microseconds>
                (currentTime - starts)};
        std::array<bool, 5> milestone{
            elapsedMicro.count() >= std::chrono::microseconds(1).count(),
            isFrameCompleted
        };
        milestone[2] = milestone[0] || milestone[1];
        executionCount++;

        std::chrono::microseconds final;
        fmt::memory_buffer elapsed{};
        if (milestone[1]) {
            final = std::chrono::duration_cast<std::chrono::microseconds>(finish - starts);
            fmt::format_to(std::back_inserter(elapsed),
                "Complete frame within {}µs...\n", final.count());
            isFrameCompleted = false;
        }
        if (milestone[2]) {
            fmt::format_to(std::back_inserter(elapsed),
                "Reaching the milestone of a second, I inform that: \n");
            fmt::format_to(std::back_inserter(elapsed),
                "Cycles performed by the components: \n");
            fmt::format_to(std::back_inserter(elapsed), "EE/VU: {}\n", eeVu.cpuAcc);
            fmt::format_to(std::back_inserter(elapsed), "PSX/BUS: {}\n", iopBus.ioAcc);

            milestone[3] = eeVu.cpuCycles < eeVu.cpuAcc;
            milestone[4] = iopBus.ioCycles < iopBus.ioAcc;

            if (milestone[3] || milestone[4]) {
                fmt::format_to(std::back_inserter(elapsed), "Expected cycles not executed: \n");
                if (milestone[3]) {
                    fmt::format_to(std::back_inserter(elapsed), "EE/VU: {}\n",
                        eeVu.cpuCycles - eeVu.cpuAcc);
                } if (milestone[4]) {
                    fmt::format_to(std::back_inserter(elapsed), "PSX/BUS: {}\n",
                        iopBus.ioCycles - iopBus.ioAcc);
                }
            }

            userLog->info("{}", fmt::join(elapsed, ""));
            eeVu.cpuAcc = 0;
            iopBus.ioAcc = 0;
        }
    }
    bool WatchStatus::get(CheckStatus status) const {
        switch (status) {
        case HasFrame:
            return isFrameCompleted.load();
        case IsRunning:
            return running.load();
        case IsMonitoring:
            return monitor.load();
        }
        return {};
    }
    void WatchStatus::set(CheckStatus status, bool value) {
        if (status == HasFrame) {
            isFrameCompleted.store(value);
        } else if (status == IsRunning) {
            running.store(value);
        } else if (status == IsMonitoring) {
            monitor.store(value);
        }
    }
    void WatchStatus::markStarts() {
        starts = std::chrono::high_resolution_clock::now();
        isFrameCompleted = true;
    }
    void WatchStatus::clearStatus() {
        isFrameCompleted.store(false);
        running.store(false);
        monitor.store(false);

        executionCount = {};
    }
}