// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <common/global.h>
#include <iop/iop_intc.h>
#include <iop/iop_core.h>

namespace cosmic::iop {
    void IopIntC::iopCheck() {
        iop->intByIntC(ctrl && (stat & mask));
    }
    void IopIntC::resetInterrupt() {
        stat = {};
        mask = {};
        ctrl = {};
    }

    void IopIntC::assertIrq(i32 id) {
        stat |= 1 << id;
        iopCheck();
    }
    u32 IopIntC::readMask() {
        return mask;
    }
    u32 IopIntC::readStat() {
#if !defined(NDEBUG)
        static std::array<std::string, 26> iopIrqStats {
            "VblankStart",
            "Gpu", // (used in PSX mode)
            "CdvdDrive",
            "Dma",
            "Timer0", // Reached
            "Timer1",
            "Timer2",
            "Sio0",
            "Sio1",
            "Spu2",
            "Pio",
            "VblankEnd",
            "Dvd", // Again?
            "PCMCIA",
            "Timer3",
            "Timer4",
            "Timer5",
            "Sio2",
            "HTR0",
            "HTR1", // All HTRx is Unknown
            "HTR2",
            "HTR3",
            "Usb",
            "EXTR",
            "FireWire",
            "FDma" // FIreWire DMA? (unknown)
        };
        if (stat < iopIrqStats.size()) {
            user->info("Stat flag being read, output value: {}", iopIrqStats[stat]);
        }
#endif
        return stat;
    }
    u32 IopIntC::readICtrl() {
        // Global interrupt disable
        const u32 ic{ctrl};
        ctrl ^= ctrl;
        iopCheck();
        return ic;
    }
    void IopIntC::wrStat(u32 st) {
        stat &= st;
        iopCheck();
    }
    void IopIntC::wrMask(u32 overMask) {
        mask = overMask;
        iopCheck();
    }
    void IopIntC::wrCtrl(u32 ic) {
        ctrl = ic & 1;
        iopCheck();
    }
}