#include <common/global.h>
#include <vm/watch.h>
namespace cosmic::vm {
    WatchStatus::WatchStatus() {
        starts = finish = std::chrono::high_resolution_clock::now();
    }
    void WatchStatus::setDesiredFrames(u8 fps) {
        if (desiredFps != fps) {
            eeVu.desired = desiredFps * 4'000'000;
            bus.desired = eeVu.desired / 2;
            psx.desired = eeVu.desired / 9;

            eeVu.acc = psx.acc = bus.acc = 0;
        }
        desiredFps = fps;
    }
    void WatchStatus::checkStatus() {
        if (eeVu.acc >= eeVu.desired)
            markStepsDone();
        if (psx.acc >= psx.desired)
            markStepsDone();
        if (bus.acc >= bus.desired)
            markStepsDone();

        if (isFrameCompleted)
            markStepsDone();
    }

    void WatchStatus::frameFinished(u64 eeCycles, u64 iopCycles, u64 busCycles) {
        eeVu.acc += eeCycles;
        psx.acc += iopCycles;
        bus.acc += busCycles;

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
            fmt::format_to(std::back_inserter(elapsed), "EE/VU: {}\n", eeVu.acc);
            fmt::format_to(std::back_inserter(elapsed), "PSX: {}\n", psx.acc);
            fmt::format_to(std::back_inserter(elapsed), "BUS: {}\n", bus.acc);

            milestone[3] = eeVu.desired < eeVu.acc;
            milestone[4] = psx.desired < psx.acc;

            if (milestone[3] || milestone[4]) {
                fmt::format_to(std::back_inserter(elapsed), "Expected cycles not executed: \n");
                if (milestone[3]) {
                    fmt::format_to(std::back_inserter(elapsed), "EE/VU: {}\n",
                        eeVu.desired - eeVu.acc);
                } if (milestone[4]) {
                    fmt::format_to(std::back_inserter(elapsed), "PSX: {}\n",
                        bus.desired - bus.acc);
                }
            }

            user->info("{}", fmt::join(elapsed, ""));
            eeVu.acc = 0;
            psx.acc = 0;
            bus.acc = 0;
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