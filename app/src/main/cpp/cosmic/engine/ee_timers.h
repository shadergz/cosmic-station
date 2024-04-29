#pragma once
#include <common/types.h>

#include <console/intc.h>
#include <vm/sched_logical.h>
namespace cosmic::vm {
    class Scheduler;
}
namespace cosmic::engine {
    struct EeHwTimer {
        u32 clocks;
        bool isEnabled;
        u16 count;
        bool gate;

        vm::CallBackId callId;
        struct {
            // Causes interruptions in the CPU when enabled
            struct {
                u16 comp;
                // Only exists for T0 and T1.
                u16 sbus;
            } values;
            struct {
                bool overflow;
                bool compare;
            } trap;
        } ctrl;
    };

    class EeTimers {
    public:
        EeTimers(std::shared_ptr<vm::Scheduler>& solver, std::shared_ptr<console::IntCInfra>& inte);
        void resetTimers();

    private:
        std::shared_ptr<vm::Scheduler> scheduler;
        std::shared_ptr<console::IntCInfra> intc;

        void timerReached(u8 raised, bool overflow);
        vm::CallBackId raiseEvent{};
        std::array<EeHwTimer, 4> timers;
    };
}
