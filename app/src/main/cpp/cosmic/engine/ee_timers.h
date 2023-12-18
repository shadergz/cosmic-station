#pragma once

#include <common/types.h>
namespace cosmic::vm {
    class Scheduler;
}
namespace cosmic::engine {
    struct EeHwTimer {
        u32 clocks;
        bool isEnabled;
    };

    class EeTimers {
    public:
        EeTimers();
        void resetTimers();

        std::shared_ptr<vm::Scheduler> clockWake;
    private:
        void timerReached(u8 raised);
        std::array<EeHwTimer, 4> timers;
    };
}
