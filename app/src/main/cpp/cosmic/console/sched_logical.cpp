#include <console/sched_logical.h>
namespace cosmic::console {
    void Scheduler::runEvents() {
        if (eeCycles.cycles < nextEventCycle)
            return;
        // Some event needs to be executed; we need to find it, execute it, and deactivate it
        for (u8 it{}; it < events.size(); it++) {
            if (events[it].runAt > nextEventCycle && !events[it].isActivated) {
                // We need to keep track of and update the nearest upcoming event after the system
                // has been updated
                nextEventCycle = std::min(events[it].runAt, static_cast<i64>(0x7fffffffull<<32));
                continue;
            }
            events[it].callback(it);
            events[it].isActivated = false;
        }
    }

    Scheduler::Scheduler() {
        events.resize(4);
    }

    void Scheduler::postMakeTimer(u32 ofMask, u8 elPos, TimerInvokable invoke) {
        TimerEvent poster{};
        poster.callback = invoke;
        poster.timer.lastUpdate = eeCycles.cycles;
        poster.runAt = eeCycles.cycles + std::numeric_limits<i64>::max();

        // Put the event ahead if its priority is lower than the one already established
        nextEventCycle = std::min(poster.runAt, nextEventCycle);
        events.emplace_back(std::move(poster));
    }
    void Scheduler::resetCycles() {
        eeCycles.highClock = 0;
        eeCycles.remain = 0;
        eeCycles.cycles = 0;

        busCycles.highClock = 0;
        busCycles.remain = 0;
        busCycles.cycles = 0;

        iopCycles.highClock = 0;
        iopCycles.remain = 0;
        iopCycles.cycles = 0;

        nextEventCycle = std::numeric_limits<i64>::max();
    }
    u32 Scheduler::getNextCycles(VirtDeviceLTimer high0) {
        static const u32 maxMips{32};
        u32 cycles{};
        if (high0 == Mips) {
            i64 delta{nextEventCycle - eeCycles.highClock};
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

    void Scheduler::updateCyclesCount() {
        eeCycles.highClock += eeCycles.cycles;
        busCycles.highClock += eeCycles.cycles >> 1;

        // The EE has a theoretical clock speed of 294MHz, while the IOP has around 33MHz!
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
