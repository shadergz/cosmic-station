#pragma once
#include <vm/sched_logical.h>
namespace cosmic::ee {
    class EeMipsCore;

    enum EmotionTrapCode {
        Gs,
        SBus,
        VBlankStart,
        VBlankEnd,
        Vif0,
        Vif1,
        Vu0,
        Vu1,
        T0, T1, T2, T3,
        SFifo,
        VuoWatchDog
    };
    class EeIntC {
    public:
        EeIntC(std::shared_ptr<EeMipsCore>& mips,
            std::shared_ptr<vm::Scheduler>& sq);

        void raiseIrq(u8 id);
        void int0Check();
    private:
        u32 intcStat,
            intcMask;
        vm::CallBackId check0Id{};
        std::shared_ptr<EeMipsCore> ee;
        std::shared_ptr<vm::Scheduler> sched;
    };
}