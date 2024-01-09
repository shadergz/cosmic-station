#include <vm/sched_logical.h>
namespace cosmic::vm {
    void Scheduler::runEvents() {
        if (eeCycles.cycles < nextEventCycle)
            return;
        // Some event needs to be executed; we need to find it, execute it, and deactivate it
        SchedulerInvokable func{};
        i64 lastRunAt;
        for (auto eve{std::begin(events)}; eve != std::end(events); eve++) {
            lastRunAt = eve->timer.runAt;
            if (lastRunAt <= nextEventCycle) {
                // If we've reached this point, we need to execute all events
                // in order, up to the current CPU cycle update
                func = eve->callback;
                func(0, false);
                eve = events.erase(eve);
                continue;
            }
            nextEventCycle = std::min(lastRunAt, static_cast<i64>(0x7fffffffull<<32));
        }
        nextEventCycle = static_cast<i64>(0x7fffffffull<<32);
    }
    Scheduler::Scheduler() {
        schedTimers.resize(4 * 4);
        schedEvents.resize(8 * 8);

        timers.resize(schedTimers.capacity());
        events.resize(schedEvents.capacity());
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
        std::list<EventSched> ee{};
        std::vector<TimerSched> te;

        events.swap(ee);
        timers.swap(te);

        schedTimers.clear();
        schedEvents.clear();
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
    CallBackId Scheduler::makeEt(bool isEvent, SchedulerInvokable invoke) {
        CallBackId ti;
        CommonSched common{};
        common.callback = invoke;
        if (isEvent) {
            TimerSched tis{};
            *dynamic_cast<CommonSched*>(&tis) = common;
            ti = schedTimers.size();

            schedTimers.push_back(tis);
        } else {
            EventSched eve{};
            *dynamic_cast<CommonSched*>(&eve) = common;
            ti = schedTimers.size();
            schedEvents.push_back(eve);
        }
        return ti;
    }
    CallBackId Scheduler::pushUpcomingEt(CallBackId id, u64 run, CallBackParam param) {
        if (schedEvents.size() < id)
            return {};
        EventSched neoEve{};
        u64 idd{};

        *dynamic_cast<CommonSched*>(&neoEve) = schedEvents[id];
        neoEve.timer.runAt = static_cast<i64>(eeCycles.cycles + run);
        neoEve.params = param;

        // Check if the new event will occur before the others
        nextEventCycle = std::min(neoEve.timer.runAt, nextEventCycle);

        idd = events.size();
        events.push_back(std::move(neoEve));
        return idd;
    }
    CallBackId Scheduler::spawnTimer(CallBackId id, u64 ovMask, CallBackParam param) {
        TimerSched clock{};
        *dynamic_cast<CommonSched*>(&clock) = schedTimers[id];

        clock.isPaused = true;
        clock.params = param;
        clock.timer.target = 0;
        clock.timer.lastUpdate = eeCycles.cycles;
        clock.overflowMask = ovMask;

        clock.childEvent = pushUpcomingEt(id, std::numeric_limits<u64>::max(),
            std::make_pair(timers.size(), false));

        u64 idd{timers.size()};
        timers.push_back(std::move(clock));
        return idd;
    }
}
