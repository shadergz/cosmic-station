#include <common/except.h>
#include <eeiv/timer/ee_timers.h>

#include <console/sched_logical.h>
namespace zenith::eeiv::timer {
    EETimers::EETimers() {}

    void EETimers::resetTimers() {
        if (!wakeUp)
            throw TimerFail("");

        for (u8 tiEn{}; tiEn != timers.size(); tiEn++) {
            timers[tiEn].clocks = 0;
            timers[tiEn].isEnabled = false;

            wakeUp->postMakeTimer(0xffff, tiEn, [this](u8 position) {
                timerReached(position);
            });
        }
    }
    void EETimers::timerReached(u8 raised) {}
}


