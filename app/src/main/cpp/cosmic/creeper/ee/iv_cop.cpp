// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <range/v3/algorithm.hpp>

#include <creeper/ee/cached_blocks.h>
#include <engine/ee_core.h>
namespace cosmic::creeper::ee {
    void MipsIvInterpreter::tlbr(Operands ops) {
        auto entry{cpu->fetchTlbFromCop(c0->GPRs.data())};
        c0->loadFromGprToTlb(*entry);
    }
    void MipsIvInterpreter::c0mfc(Operands ops) {
        if (!ops.rd)
            return;
        auto res = c0->mfc0(ops.rd);
        *(cpu->gprAt<u32>(ops.rt)) = res;
    }
    void MipsIvInterpreter::c0mtc(Operands ops) {
        std::array<u32*, 2> c0mop{};
        c0mop[0] = cpu->gprAt<u32>(ops.rd);
        c0mop[1] = cpu->gprAt<u32>(ops.rt);

        if (*c0mop[0] != 14 && *c0mop[0] != 30) {
        }
        c0->mtc0(static_cast<u8>(*c0mop[0]), *c0mop[1]);
    }

    // bc0f, bc0t, bc0fl, bc0tl
    void MipsIvInterpreter::copbc0tf(Operands ops) {
        const std::array<u8, 4> likely{0, 0, 1, 1};
        const std::array<u8, 4> opTrue{0, 1, 0, 1};
        u8 variant{static_cast<u8>(ops.pa16[1] & 0x1f)};

        bool condEval;
        if (opTrue[variant])
            condEval = c0->getCondition();
        else
            condEval = !c0->getCondition();
        if (likely[variant])
            cpu->branchOnLikely(condEval, ops.sins & 0xffff);
        else
            cpu->branchByCondition(condEval, ops.sins & 0xffff);
    }
    void MipsIvInterpreter::tlbwi(Operands ops) {
        cpu->setTlbByIndex();
    }
    void MipsIvInterpreter::eret(Operands ops) {
        if (c0->status.error) {
            cpu->chPc(c0->errorPC);
            c0->status.error = false;
        } else {
            cpu->chPc(c0->ePC);
            c0->status.exception = false;
        }
        // This will set the last PC value to PC, and the PC to PC - 4
        cpu->chPc(cpu->eePc--);
        struct CoreHoles {
            u32 iAddr, eAddr;
        };
        // https://forums.pcsx2.net/archive/index.php/thread-13784-3.html
        static std::array<CoreHoles, 1> holes{
            // (BIFC0) Speed Hack
            {{0x81fc0, 0x81fe0}}
        };
        ranges::for_each(holes, [&](auto region) {
            if (*cpu->eePc >= region.iAddr && *cpu->eePc < region.eAddr)
                cpu->haltCpu();
        });

        c0->redoTlbMapping();
    }

    void MipsIvInterpreter::ei(Operands ops) {
        c0->enableInt();
    }
    void MipsIvInterpreter::di(Operands ops) {
        c0->disableInt();
    }
}