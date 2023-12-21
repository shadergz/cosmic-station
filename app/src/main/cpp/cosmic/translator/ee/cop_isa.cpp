// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <translator/ee/mipsiv_interpreter.h>
#include <engine/ee_core.h>
namespace cosmic::translator::ee {
    void MipsIvInterpreter::tlbr(Operands ops) {
        auto entry{mainMips->fetchTlbFromCop(mainMips->ctrl0.GPRs.data())};
        mainMips->ctrl0.loadFromGprToTlb(entry.safeRaw->get());
    }
    void MipsIvInterpreter::c0mfc(Operands ops) {
        u32 res;
        res = mainMips->ctrl0.mfc0(ops.rd);
        *(mainMips->gprAt<u32>(ops.rt)) = res;
    }
    void MipsIvInterpreter::c0mtc(Operands ops) {
        std::array<u32*, 2> c0mop{};
        c0mop[0] = mainMips->gprAt<u32>(ops.rd);
        c0mop[1] = mainMips->gprAt<u32>(ops.rt);

        if (*c0mop[0] != 14 && *c0mop[0] != 30)
            ;
        mainMips->ctrl0.mtc0(static_cast<u8>(*c0mop[0]), *c0mop[1]);
    }

    // bc0f, bc0t, bc0fl, bc0tl
    void MipsIvInterpreter::copbc0tf(Operands ops) {
        const static std::array<u8, 4> likely{0, 0, 1, 1};
        const static std::array<u8, 4> opTrue{0, 1, 0, 1};
        u8 variant{static_cast<u8>(ops.operation.pa16[1] & 0x1f)};

        bool condEval;
        if (opTrue[variant])
            condEval = mainMips->ctrl0.getCondition();
        else
            condEval = !mainMips->ctrl0.getCondition();
        if (likely[variant])
            mainMips->branchOnLikely(condEval, ops.operation.sins & 0xffff);
        else
            mainMips->branchByCondition(condEval, ops.operation.sins & 0xffff);
    }
    void MipsIvInterpreter::tlbwi(Operands ops) {
        mainMips->setTlbByIndex();
    }
    void MipsIvInterpreter::eret(Operands ops) {
        raw_reference<engine::copctrl::CoProcessor0> c0{mainMips->ctrl0};
        if (c0->status.error) {
            mainMips->chPc(c0->errorPC);
            c0->status.error = false;
        } else {
            mainMips->chPc(c0->ePC);
            c0->status.exception = false;
        }
        // This will set the last PC value to PC, and the PC to PC - 4
        mainMips->chPc(mainMips->eePc--);
        mainMips->updateTlb();
    }

    void MipsIvInterpreter::ei(Operands ops) {
        mainMips->ctrl0.enableInt();
    }
    void MipsIvInterpreter::di(Operands ops) {
        mainMips->ctrl0.disableInt();
    }
}