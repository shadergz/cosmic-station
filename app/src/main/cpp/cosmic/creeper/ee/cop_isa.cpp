// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <creeper/ee/mipsiv_cached.h>
#include <engine/ee_core.h>
namespace cosmic::creeper::ee {
    void MipsIvInterpreter::tlbr(Operands ops) {
        auto entry{cpu->fetchTlbFromCop(control->GPRs.data())};
        control->loadFromGprToTlb(*entry);
    }
    void MipsIvInterpreter::c0mfc(Operands ops) {
        u32 res;
        if (ops.rd == 0)
            return;
        res = control->mfc0(ops.rd);
        *(cpu->gprAt<u32>(ops.rt)) = res;
    }
    void MipsIvInterpreter::c0mtc(Operands ops) {
        std::array<u32*, 2> c0mop{};
        c0mop[0] = cpu->gprAt<u32>(ops.rd);
        c0mop[1] = cpu->gprAt<u32>(ops.rt);

        if (*c0mop[0] != 14 && *c0mop[0] != 30)
            ;
        control->mtc0(static_cast<u8>(*c0mop[0]), *c0mop[1]);
    }

    // bc0f, bc0t, bc0fl, bc0tl
    void MipsIvInterpreter::copbc0tf(Operands ops) {
        const static std::array<u8, 4> likely{0, 0, 1, 1};
        const static std::array<u8, 4> opTrue{0, 1, 0, 1};
        u8 variant{static_cast<u8>(ops.pa16[1] & 0x1f)};

        bool condEval;
        if (opTrue[variant])
            condEval = control->getCondition();
        else
            condEval = !control->getCondition();
        if (likely[variant])
            cpu->branchOnLikely(condEval, ops.sins & 0xffff);
        else
            cpu->branchByCondition(condEval, ops.sins & 0xffff);
    }
    void MipsIvInterpreter::tlbwi(Operands ops) {
        cpu->setTlbByIndex();
    }
    void MipsIvInterpreter::eret(Operands ops) {
        RawReference<engine::copctrl::CtrlCop> c0{cpu->cop0};
        if (c0->status.error) {
            cpu->chPc(c0->errorPC);
            c0->status.error = false;
        } else {
            cpu->chPc(c0->ePC);
            c0->status.exception = false;
        }
        // This will set the last PC value to PC, and the PC to PC - 4
        cpu->chPc(cpu->eePc--);
        cpu->updateTlb();
    }

    void MipsIvInterpreter::ei(Operands ops) {
        control->enableInt();
    }
    void MipsIvInterpreter::di(Operands ops) {
        control->disableInt();
    }
}