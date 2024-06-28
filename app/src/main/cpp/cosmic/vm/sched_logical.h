#pragma once
#include <vector>
#include <list>

#include <common/types.h>
namespace cosmic::vm {
    using SchedulerInvokable = std::function<void(u64, bool)>;
    using CallBackId = size_t;
    using CallBackParam = std::tuple<u64, bool>;

    constexpr u64 eeClockRate{294912000}; //294.912 MHz
    constexpr u64 busClockRate{eeClockRate / 2};
    constexpr u64 iopClockRate{eeClockRate / 8};

    enum TimerSet {
        Pause,
        ClockRate,
        Counter,
        Target,
        IntMask
    };

    enum AffinityControl {
        EmotionEngine = 0x3,
        GS = 0x6,
        VUs = 0x7
    };
    enum AffinityModels {
        Normal,
        PrioritizeVectors,
        GraphicsFirst,
    };
    struct BaseSched {
        BaseSched() {
        }
        u64 target;
        u64 insideCycleCount; // Time to run
        std::tuple<u64, bool> params;
        SchedulerInvokable callback;

    };
    struct TimerSched : BaseSched {
        bool isPaused;
        bool canOverflow,
            canTarget;
        bool hasTarget;
        CallBackId childEvent;
        u64 clockRate,
            counter,
            remainderClocks,
            overflowMask,
            target,
            lastUpdate;
    };
    struct EventSched : BaseSched {
        bool pulse;
    };

    class Scheduler {
    public:
        using EventIterator = std::list<EventSched>::iterator;

        struct MachineCycles {
            u64 highClock;
            u64 remain;
            u32 cycles;
        };
        enum VirtDeviceLTimer : u8 {
            Mips,
            IOP,
            Bus
        };

        Scheduler();
        void resetCycles();

        u32 getNextCycles(VirtDeviceLTimer high0);
        void updateCyclesCount();

        [[nodiscard]] CallBackId createSchedTick(bool isEvent = true,
            SchedulerInvokable invoke = {});
        std::optional<CallBackId> placeTickedTask(CallBackId sid, u64 magic,
            CallBackParam param, bool isEvent = false);

        void modifyTimerSet(CallBackId sid, TimerSet setMode, const std::vector<u64>& set);
        std::vector<u64> readTimerSet(CallBackId sid, TimerSet setMode);
        EventIterator searchIdEvent(CallBackId sid);

        void runTasks();

        u32 affinity{};
    private:
        MachineCycles eeCycles,
            busCycles,
            iopCycles;
        u64 nearestEventCycle;

        std::vector<BaseSched> schedTimers;
        std::vector<BaseSched> schedEvents;

        std::vector<TimerSched> timers;
        std::list<EventSched> events;
    };
}
