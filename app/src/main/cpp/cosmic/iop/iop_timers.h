#pragma once

#include <common/types.h>

#include <console/intc.h>
#include <vm/sched_logical.h>
namespace cosmic::iop {
    struct TimerControl {
        bool useGate;
        u8 gateMode;
        bool zeroReturn;
        bool cmpIntEnb;
        bool overIntEnb;
        bool repeatInt;
        // toggle bit 10 (intEnable) on IRQs if bit 6 (repeatInt) is set.
        bool toggleInt;
        bool intEnable;
        bool externSignal;
        u8 preScale;
        bool cmpInterrupt;
        bool overInterrupt;
        bool started;
    };

    struct IopTimer {
        u64 counter;
        TimerControl control;
        u64 target;
    };

    class IopTimers {
    public:
        IopTimers(std::shared_ptr<vm::Scheduler>& source,
            std::shared_ptr<console::IntCInfra>& infra);

        void resetIoTimers();
        u16 readCtrl(u64 index);
        void writeCounter(u64 index, u32 value);
        void writeCtrl(u64 index, u16 value);
        void writeTarget(u64 index, u32 value);
    private:
        void timerIrqTest(u64 index, bool overflow);
        [[clang::always_inline]] void clearTimerCounter(u64 index);

        std::array<IopTimer, 6> ioTimers;
        std::array<vm::CallBackId, 6> intEvents;
        vm::CallBackId timerIntEnbId;

        std::shared_ptr<vm::Scheduler> ioSched;
        std::shared_ptr<console::IntCInfra> infra;
    };
}

