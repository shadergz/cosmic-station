// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>
namespace cosmic::fuji {
    IV_FUJI_SUPER_ASM(tlbr) {
        auto entry{mainMips.fetchTLBFromCop(mainMips.ctrl0.GPRs.data())};
        mainMips.ctrl0.loadGPRTLB(std::ref(*entry));
    }
    IV_FUJI_SUPER_ASM(c0mfc) {
        u32 res;
        res = mainMips.ctrl0.mfc0(ops.fir);
        *(mainMips.gprAt<u32>(ops.sec)) = res;
    }
    IV_FUJI_SUPER_ASM(c0mtc) {
        std::array<u32*, 2> c0mop{};
        c0mop[0] = mainMips.gprAt<u32>(ops.fir);
        c0mop[1] = mainMips.gprAt<u32>(ops.sec);

        if (*c0mop[0] != 14 && *c0mop[0] != 30)
            ;
        mainMips.ctrl0.mtc0(static_cast<u8>(*c0mop[0]), *c0mop[1]);
    }

    // bc0f, bc0t, bc0fl, bc0tl
    IV_FUJI_SUPER_ASM(copbc0tf) {
        const static std::array<u8, 4> likely{0, 0, 1, 1};
        const static std::array<u8, 4> opTrue{0, 1, 0, 1};
        u8 variant{static_cast<u8>(ops.operation.pa16[1] & 0x1f)};

        bool condEval;
        if (opTrue[variant])
            condEval = mainMips.ctrl0.getCondition();
        else
            condEval = !mainMips.ctrl0.getCondition();
        if (likely[variant])
            mainMips.branchOnLikely(condEval, ops.operation.sins & 0xffff);
        else
            mainMips.branchByCondition(condEval, ops.operation.sins & 0xffff);
    }
    IV_FUJI_SUPER_ASM(tlbwi) {
        mainMips.setTlbByIndex();
    }
    IV_FUJI_SUPER_ASM(eret) {
        raw_reference<eeiv::copctrl::CoProcessor0> c0{mainMips.ctrl0};
        if (c0->status.error) {
            mainMips.chPC(c0->errorPC);
            c0->status.error = false;
        } else {
            mainMips.chPC(c0->ePC);
            c0->status.exception = false;
        }
        // This will set the last PC value to PC, and the PC to PC - 4
        mainMips.chPC(mainMips.eePC--);
        mainMips.updateTlb();
    }
    IV_FUJI_SUPER_ASM(ei) {
        mainMips.ctrl0.enableInt();
    }
    IV_FUJI_SUPER_ASM(di) {
        mainMips.ctrl0.disableInt();
    }
}