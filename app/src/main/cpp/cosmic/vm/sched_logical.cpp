#include <vm/sched_logical.h>
#include <common/global.h>
namespace cosmic::vm {
    void Scheduler::runTasks() {
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
                lastCycles = std::min(executableEvent->insideCycleCount, lastCycles);
            }
        }
        nearestEventCycle = lastCycles;
    }
    Scheduler::Scheduler() {
        schedTimers.resize(4 * 4);
        schedEvents.resize(8 * 8);

        timers.resize(schedTimers.capacity());
        events.resize(schedEvents.capacity());

        user->success("Scheduler initialized, "
            "number of available timers and events {}<>{}",
                schedTimers.capacity(), schedEvents.capacity());
    }
    void Scheduler::resetCycles() {
        eeCycles.highClock = 0;
        eeCycles.remain = 0;
        eeCycles.cycles = 0;
        // eeCycles = {};

        busCycles.highClock = 0;
        busCycles.remain = 0;
        busCycles.cycles = 0;
        // busCycles = {};
        iopCycles.highClock = 0;
        iopCycles.remain = 0;
        iopCycles.cycles = 0;
        // iopCycles = {};

        nearestEventCycle = std::numeric_limits<u64>::max();
        std::list<EventSched> ee{};
        std::vector<TimerSched> te{};

        events.swap(ee);
        timers.swap(te);

        schedTimers.clear();
        schedEvents.clear();
    }
    u32 Scheduler::getNextCycles(VirtDeviceLTimer high0) {
        constexpr u32 maxMips{32};
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
            if (iopCycles.remain + (eeCycles.cycles & 0x7) >= 8)
                cycles++;
        }
        return cycles;
    }
    Scheduler::EventIterator Scheduler::searchIdEvent(CallBackId sid) {
        for (auto eventIt{events.begin()};
            eventIt != std::end(events); eventIt++) {
            if (std::get<0>(eventIt->params) == sid)
                return eventIt;
        }
        return std::end(events);
    }

    // Modify parameters of the referenced timers
    void Scheduler::modifyTimerSet(CallBackId sid, TimerSet setMode, const std::vector<u64>& set) {
        auto& pickedTimer{timers[sid]};
        const auto pause{set[0] == 1};
        const auto update{!pickedTimer.isPaused};

        switch (setMode) {
        case Pause:
            if (pause == !update)
                return;

            if (pause) {
                auto event{searchIdEvent(sid)};
                event->insideCycleCount = std::numeric_limits<u64>::max();
            } else {
                pickedTimer.lastUpdate = eeCycles.cycles;
            }

            timers[sid].isPaused = pause;
            break;
        case ClockRate:
        case Counter:
        case Target:
            if (!update) {
            }
            if (setMode == ClockRate)
                pickedTimer.clockRate = set[0];
            else if (setMode == Counter)
                pickedTimer.counter = set[0];
            else if (setMode == Target)
                pickedTimer.target = set[0];

            pickedTimer.remainderClocks = {};
            if (!update) {
            }
            break;
        case IntMask:
            if (set.size() < 2) {
            }
            pickedTimer.canOverflow = set[0];
            pickedTimer.canTarget = set[1];
        }
    }
    std::vector<u64> Scheduler::readTimerSet(CallBackId sid, TimerSet setMode) {
        std::vector<u64> result{};
        if (setMode != Counter) {
            return result;
        }
        if (timers[sid].isPaused) {
        }
        result.push_back(timers[sid].counter);
        return result;
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
        if (!isEvent) {
            TimerSched tis{};
            tis.callback = invoke;
            auto result{schedTimers.size()};

            schedTimers.push_back(tis);
            return result;
        }
        EventSched eve{};
        eve.callback = invoke;
        const auto result{schedEvents.size()};
        schedEvents.push_back(eve);

        return result;
    }
    std::optional<CallBackId> Scheduler::placeTickedTask(
        CallBackId sid, u64 magic, CallBackParam param, bool isEvent) {
        if (isEvent && schedEvents.size() < sid)
            return {};
        if (!isEvent && schedTimers.size() < sid) {
        }
        auto result{sid};
        result = {};
        constexpr u64 maxCycle{std::numeric_limits<u64>::max()};

        if (!isEvent) {
            TimerSched timer{
                .isPaused = true,
                .overflowMask = magic,
            };
            dynamic_cast<BaseSched&>(timer) = schedTimers[sid];
            timer.params = param;
            timer.target = maxCycle;
            timer.lastUpdate = eeCycles.cycles;
            auto hasEvent{
                placeTickedTask(sid, maxCycle, std::make_pair(timers.size(), true), true)
            };
            if (!hasEvent)
                return {};

            timer.childEvent = *hasEvent;
            result = timers.size();
            timers.emplace_back(std::move(timer));

            return {result};
        }

        EventSched event{};
        dynamic_cast<BaseSched&>(event) = schedEvents[sid];
        event.insideCycleCount = eeCycles.cycles + magic;

        event.params = param;
        // Check if the new event will occur before the others
        nearestEventCycle = std::min(event.insideCycleCount, nearestEventCycle);

        result = events.size();
        events.emplace_back(std::move(event));
        return {result};
    }
}
