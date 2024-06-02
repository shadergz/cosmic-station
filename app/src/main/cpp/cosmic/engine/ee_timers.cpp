#include <common/except.h>
#include <engine/ee_timers.h>
#include <engine/ee_intc.h>
#include <console/intc.h>

namespace cosmic::engine {
    EeTimers::EeTimers(std::shared_ptr<vm::Scheduler>& solver,
        std::shared_ptr<console::IntCInfra>& inte) :
            scheduler(solver), intc(inte) {
        std::memset(&timers, 0, sizeof(timers));

    }
    // PAL:  312 scanlines per frame (VBOFF: 286 | VBON: 26)
    // NTSC: 262 scanlines per frame (VBOFF: 240 | VBON: 22)

    // PAL:  9436 BUSCLK cycles per scanline
    // NTSC: 9370 BUSCLK cycles per scanline
    void EeTimers::sysCtrlGate(bool vb, bool high) {
        for (u8 ti{}; ti < timers.size(); ti++) {
            Ref<HwTimer> clocked{timers[ti]};
            if (!clocked->isGateEnabled && clocked->withVbSync != vb)
                continue;

            switch (clocked->gateMode) {
            case ActivateGate:
                clocked->gated = high;
                break;
            case ResetGateWhenHigh:
                if (high)
                    clocked->count = 0;
                break;
            case ResetGateWhenLow:
                if (!high)
                    clocked->count = 0;
                clocked->gated = true;
                break;
            case ResetGateWithDiffer:
                // 3=Reset counter for high<->low gate transitions
                if (clocked->gated == high)
                    continue;
                clocked->count = 0;
                clocked->gated = high;
            }
        }
    }
    void EeTimers::resetTimers() {
        for (u8 chronos = {}; chronos != timers.size(); chronos++) {
            // Not necessary perhaps, it will depend on the implementation
            timers.at(chronos) = {};
            timers[chronos].count = {};
        }
        raiseEvent = scheduler->createSchedTick(false,
            [this](u8 rice, bool over) {
                raiseClkTrigger(rice, over);
        });
        const u16 compareDiff{0xffff};
        for (u8 idx = {}; idx != timers.size(); idx++)
            timers[idx].callId = scheduler->addTimer(
                raiseEvent, compareDiff, std::make_tuple(idx, false));
    }
    void EeTimers::raiseClkTrigger(u8 raised, bool overflow) {
        // This function is responsible for enabling the clock timer exception;
        // it should ensure that the 'overflow' flag is enabled
        static u8 base{};

        base = engine::T0 + raised;
        auto& ticked{timers.at(raised)};
        if (!overflow) {
            if (ticked.clearCountWithDiff) {
                ticked.count = {};
            }
            if (!ticked.compare) {
                ticked.compare = true;
                intc->trapIrq(console::EeInt, base);
            }
        } else if (!ticked.overflow) {
            ticked.overflow = true;
            intc->trapIrq(console::EeInt, base);
        }
    }
}

