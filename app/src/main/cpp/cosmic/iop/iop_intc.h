#pragma once

#include <common/types.h>
// 1F801070h I_STAT - Interrupt status register (R=Status, W=Acknowledge)
// 1F801074h I_MASK - Interrupt mask register (R/W)

namespace cosmic::iop {
    class IoMipsCore;
    class IopIntC {
    public:
        IopIntC(std::shared_ptr<IoMipsCore>& mips) :
            iop(mips) {}
        void iopCheck();
        void resetInterrupt();

        void assertIrq(i32 id);
        u32 readStat();
        u32 readMask();
        u32 readICtrl();

        void wrStat(u32 st);
        void wrMask(u32 overMask);
        void wrCtrl(u32 ic);
        // Status: Read I_STAT (1=IRQ raised)
        // Mask: Read/Write I_MASK (0=Disabled 1=Enabled)
        u32 stat, mask, ctrl;
    private:
        std::shared_ptr<IoMipsCore> iop;
    };
}