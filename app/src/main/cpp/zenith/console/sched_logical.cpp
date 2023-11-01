#include <console/sched_logical.h>

namespace zenith::console {
    Scheduler::Scheduler() {}

    void Scheduler::cleanCycles() {
        eeCycles.highClock = 0;
        eeCycles.remain = 0;

        nextEventCycle = std::numeric_limits<u64>::max();
    }
    u32 Scheduler::getNextCycles(VirtDeviceLTimer high0) {
        static const u32 maxMips{32};
        u32 cycles{};
        if (high0 == Mips) {
            u64 delta{nextEventCycle - eeCycles.highClock};
            if (eeCycles.highClock + maxMips <= nextEventCycle) {
                eeCycles.cycles = maxMips;
            } else {
                if (delta)
                    eeCycles.cycles = static_cast<u32>(delta);
                else
                    eeCycles.cycles = 0;
            }
            cycles = eeCycles.cycles;
        } else if (high0 == Bus) {
            cycles = eeCycles.cycles >> 1;
            if (busCycles.remain && (eeCycles.cycles & 0x1))
                cycles++;
        } else if (high0 == IOP) {
            cycles = eeCycles.cycles >> 3;
            if (iopCycles.remain && (eeCycles.cycles & 0x7) >= 8)
                cycles++;
        }
        return cycles;
    }

    void Scheduler::updateCycleCounts() {
        eeCycles.highClock += eeCycles.cycles;
        busCycles.highClock += eeCycles.cycles >> 1;

        // The EE has a theoretical clock speed of 333MHz, while the IOP has around 36MHz!
        // The shift-right used here will cut down up to 90 or 80 of the cycles used by the EE
        iopCycles.highClock += eeCycles.cycles >> 3;

        busCycles.remain += eeCycles.cycles & 1;
        if (busCycles.remain > 1) {
            busCycles.highClock++;
            busCycles.remain = 0;
        }
        iopCycles.remain += eeCycles.cycles & 7;
        if (iopCycles.remain >= 8) {
            iopCycles.highClock++;
            iopCycles.remain -= 8;
        }
    }
}

