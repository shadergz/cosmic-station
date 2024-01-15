#pragma once

#include <chrono>
#include <common/types.h>
namespace cosmic::vm {
    enum CheckStatus {
        HasFrame,
        IsRunning,
        IsMonitoring
    };
    class WatchStatus {
    public:
        WatchStatus();
        void checkStatus();
        void markStarts();
        void markStepsDone();

        void setDesiredFrames(u8 fps);
        void frameFinished(u64 eeCycles, u64 iopCycles, u64 busCycles);
        auto getExecutionCount() const {
            return executionCount.load();
        }

        bool get(CheckStatus status) const;
        void set(CheckStatus status, bool value);

        void clearStatus();
        std::atomic<bool> isFrameCompleted{false};
        std::atomic<bool> running{false};
        std::atomic<bool> monitor{false};

        std::atomic<u64> executionCount{};
    private:
        u8 desiredFps{};
        struct {
            u64 desired;
            u64 acc;
        } eeVu, psx, bus;

        std::chrono::time_point<std::chrono::high_resolution_clock> starts;
        std::chrono::time_point<std::chrono::high_resolution_clock> finish;
    };
}