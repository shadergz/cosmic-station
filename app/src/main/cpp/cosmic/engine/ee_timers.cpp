#include <common/except.h>
#include <engine/ee_timers.h>
#include <engine/ee_intc.h>

namespace cosmic::engine {
    EeTimers::EeTimers(std::shared_ptr<vm::Scheduler>& solver,
        std::shared_ptr<console::IntCInfra>& inte) :
            scheduler(solver), intc(inte) {
    }
    void EeTimers::resetTimers() {
        for (u8 tt = {}; tt != timers.size(); tt++) {
            timers.at(tt) = {};
        }
        raiseEvent = scheduler->createSchedTick(false,[this](u8 position, bool ov) {
            timerReached(position, ov);
        });
        for (u8 idx = {}; idx != timers.size(); idx++) {
            timers[idx].callId = scheduler->addTimer(raiseEvent, 0xffff,
                std::make_tuple(idx, false));
        }
    }
    void EeTimers::timerReached(u8 raised, bool overflow) {
        // This function is responsible for enabling the clock timer exception;
        // it should ensure that the 'overflow' flag is enabled
        static u8 base{};

        base = engine::T0 + raised;
        auto timer{std::addressof(timers.at(raised))};
        if (!overflow) {
            bool compare{timer->ctrl.trap.compare};
            if (compare) {
                timer->ctrl.trap.compare = true;
                intc->trapIrq(console::EeInt, base);
            }
        }
        bool woutOverflow{!timer->ctrl.trap.overflow};
        bool shouldTrap{woutOverflow && overflow};
        if (shouldTrap) {
            timer->ctrl.trap.overflow = true;
            intc->trapIrq(console::EeInt, base);
        }
    }
}


