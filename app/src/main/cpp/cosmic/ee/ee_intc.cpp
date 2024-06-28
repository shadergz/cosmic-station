#include <ee/ee_intc.h>
#include <ee/ee_core.h>

namespace cosmic::ee {
    EeIntC::EeIntC(std::shared_ptr<EeMipsCore>& mips, std::shared_ptr<vm::Scheduler>& sq) :
        ee(mips),
            sched(sq) {
        intcStat = {};
        intcMask = {};
        check0Id = sched->createSchedTick(true,
            [this](u64 unused0, bool unused1) {
            int0Check();
        });
    }
    void EeIntC::resetEeInterrupt() {
        intcStat = {};
        intcMask = {};
    }

    void EeIntC::raiseIrq(u8 id) {
        intcStat |= (1 << id);

        // Some games utilize a wait-for-VBLANK loop in which they continuously check the INTC_STAT
        // while a VBLANK interrupt handler is active
        sched->placeTickedTask(check0Id, 0x8, {}, true);
    }
    void EeIntC::int0Check() {

    }
}