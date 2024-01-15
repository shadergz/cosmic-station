#include <vm/sched_logical.h>
namespace cosmic::vm {
    void Scheduler::runEvents() {
        if (eeCycles.cycles < nearestEventCycle)
            return;
        // Some event needs to be executed; we need to find it, execute it, and deactivate it
        SchedulerInvokable executable{};
        u64 lastCycles{0x7fffffffull << 32};
        for (auto executableEvent{std::begin(events)}; executableEvent != std::end(events); executableEvent++) {
            if (lastCycles <= nearestEventCycle) {
                // If we've reached this point, we need to execute all events
                // in order, up to the current CPU cycle update
                executable = executableEvent->callback;
                executable(std::get<0>(executableEvent->params),
                    std::get<1>(executableEvent->params));
                executableEvent = events.erase(executableEvent);
            } else {
                lastCycles = std::min(executableEvent->withCycles, lastCycles);
            }
        }
        nearestEventCycle = lastCycles;
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

        nearestEventCycle = std::numeric_limits<u64>::max();
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
            u64 delta{nearestEventCycle - eeCycles.highClock};
            if (eeCycles.highClock + maxMips <= nearestEventCycle) {
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
    CallBackId Scheduler::createSchedTick(bool isEvent, SchedulerInvokable invoke) {
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
    CallBackId Scheduler::addEvent(CallBackId eventId, u64 run, CallBackParam param) {
        if (schedEvents.size() < eventId)
            return {};
        EventSched event{};
        u64 eid;

        *dynamic_cast<CommonSched*>(&event) = schedEvents[eventId];
        event.withCycles = eeCycles.cycles + run;
        event.params = param;

        // Check if the new event will occur before the others
        nearestEventCycle = std::min(event.withCycles, nearestEventCycle);

        eid = events.size();
        events.push_back(std::move(event));
        return eid;
    }
    CallBackId Scheduler::addTimer(CallBackId timerEventId, u64 ovMask, CallBackParam param) {
        if (timerEventId >= schedTimers.size())
            ;
        TimerSched timer{
            .isPaused = true,
            .overflowMask = ovMask
        };
        *dynamic_cast<CommonSched*>(&timer) = schedTimers[timerEventId];
        timer.params = param;
        timer.target = 0;
        timer.lastUpdate = eeCycles.cycles;

        timer.childEvent = addEvent(timerEventId, std::numeric_limits<u64>::max(),
            std::make_pair(timers.size(), false));

        u64 tid{timers.size()};
        timers.push_back(std::move(timer));
        return tid;
    }
}
