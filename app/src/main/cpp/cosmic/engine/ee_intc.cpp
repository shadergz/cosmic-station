#include <engine/ee_intc.h>
#include <engine/ee_core.h>

namespace cosmic::engine {
    EeIntC::EeIntC(std::shared_ptr<EeMipsCore>& mips, std::shared_ptr<vm::Scheduler>& sq) :
        ee(mips),
            sched(sq) {
        intcStat = 0;
        intcMask = 0;
        check0Id = sched->createSchedTick(true, [this](u64 unused0, bool unused1) {
            int0Check();
        });
    }

    void EeIntC::raiseIrq(u8 id) {
        // Some games utilize a wait-for-VBLANK loop in which they continuously check the INTC_STAT
        // while a VBLANK interrupt handler is active
        sched->addEvent(check0Id, 0x8, {});
    }
    void EeIntC::int0Check() {
    }
}