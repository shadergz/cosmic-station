#pragma once
#include <common/types.h>
namespace zenith::console {
    class Scheduler {
    public:
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
        void cleanCycles();
        u32 getNextCycles(VirtDeviceLTimer high0);
        void updateCyclesCount();

    private:
        MachineCycles eeCycles,
            busCycles,
            iopCycles;
        u64 nextEventCycle;
    };
}
