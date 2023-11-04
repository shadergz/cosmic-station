#pragma once

#include <common/types.h>
namespace zenith::console {
    class Scheduler;
}
namespace zenith::eeiv::timer {
    struct EEHwTimer {
        u32 clocks;
        bool isEnabled;
    };

    class EETimers {
    public:
        EETimers();
        void resetTimers();

        std::shared_ptr<console::Scheduler> wakeUp;
    private:
        void timerReached(u8 raised);
        std::array<EEHwTimer, 4> timers;
    };
}
