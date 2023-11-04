#pragma once
#include <list>

#include <common/types.h>
namespace zenith::console {
    using TimerInvokable = std::function<void(u8)>;
    struct TimerTask {
        i64 lastUpdate;
    };
    struct TimerEvent {
        TimerTask timer;
        TimerInvokable callback;
        i64 runAt;
        bool isActivated;
    };

    class Scheduler {
    public:
        struct MachineCycles {
            u32 highClock;
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

        std::list<TimerEvent> events;
        void postMakeTimer(u32 ofMask, u8 elPos, TimerInvokable invoke);
        void runEvents();
    private:
        MachineCycles eeCycles,
            busCycles,
            iopCycles;
        i64 nextEventCycle;
    };
}
