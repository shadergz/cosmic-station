#pragma once
#include <common/types.h>

#include <console/intc.h>
#include <vm/sched_logical.h>
namespace cosmic::vm {
    class Scheduler;
}
namespace cosmic::engine {
    struct TimerInt {
        // Causes interruptions in the CPU when enabled
        // TN_COMP
        u16 tnComp;
        // Only exists for T0 and T1.
        u16 tnHold;
        u16 sbus;
    };
    enum GateMode {
        ActivateGate,
        ResetGateWhenHigh,
        ResetGateWhenLow,
        ResetGateWithDiffer,
    };

    struct HwTimer {
        u32 clocks;
        // Count while gate not active
        u32 count;
        // All the EE timers could be deactivated by a locked gate
        bool gated;
        bool isGateEnabled;
        GateMode gateMode;
        bool withVbSync;
        bool clearCountWithDiff;

        operator bool() const {
            return gated || !isEnabled;
        }
        vm::CallBackId callId;
        struct {
            TimerInt values;
            bool overflow;
            bool isOfEnabled;
            bool compare;
            bool cmpIsEnabled;
            bool isEnabled;
        };
    };

    class EeTimers {
    public:
        EeTimers(std::shared_ptr<vm::Scheduler>& solver, std::shared_ptr<console::IntCInfra>& inte);
        void resetTimers();
    private:
        std::shared_ptr<vm::Scheduler> scheduler;
        std::shared_ptr<console::IntCInfra> intc;

        void raiseClkTrigger(u8 raised, bool overflow);
        void sysCtrlGate(bool hasVSync, bool high);
        vm::CallBackId raiseEvent{};
        std::array<HwTimer, 4> timers;
    };
}
