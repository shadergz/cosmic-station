// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <range/v3/algorithm.hpp>

#include <creeper/cached_blocks.h>
#include <ee/ee_core.h>
namespace cosmic::creeper {

    void MipsIvInterpreter::tlbr(Operands ops) {
        auto& entry{cpu->fetchTlbFromCop(c0->GPRs.data())};
        c0->loadFromGprToTlb(entry);
    }
    void MipsIvInterpreter::c0mfc(Operands ops) {
        if (!ops.rd)
            return;
        auto res{c0->mfc0(ops.rd)};
        cpu->GPRs[ops.rt].words[0] = res;
    }
    void MipsIvInterpreter::c0mtc(Operands ops) {
        std::array<u32, 2> c0mop{
            cpu->GPRs[ops.rd].words[0],
            cpu->GPRs[ops.rt].words[0]
        };
        if (c0mop[0] && c0mop[0] != 14) {
        }
        if (c0mop[1] && c0mop[1] != 30) {

        }
        u32 posDest{c0mop[0]};
        u32 setValue{c0mop[1]};
        c0->mtc0(static_cast<u8>(posDest), setValue);
    }

    // bc0f, bc0t, bc0fl, bc0tl
    void MipsIvInterpreter::copbc0tf(Operands ops) {
        const std::array<u8, 4> likely{0, 0, 1, 1};
        const std::array<u8, 4> opTrue{0, 1, 0, 1};
        auto variant{static_cast<u8>(ops.pa16[1] & 0x1f)};

        bool condEval;
        if (opTrue[variant])
            condEval = c0->getCondition();
        else
            condEval = !c0->getCondition();
        if (likely[variant])
            cpu->branchOnLikely(condEval, signedGetOffset(ops));
        else
            cpu->branchByCondition(condEval, signedGetOffset(ops));
    }
    void MipsIvInterpreter::tlbwi(Operands ops) {
        cpu->setTlbByIndex();
    }
    void MipsIvInterpreter::eret(Operands ops) {
        if (c0->status.error) {
            cpu->chPc(c0->errorPc);
            c0->status.error = false;
        } else {
            cpu->chPc(c0->ePc);
            c0->status.exception = false;
        }
        // This will set the last PC value to PC, and the PC to PC - 4
        cpu->chPc(cpu->eePc--);
        struct CoreHoles {
            u32 iAddr, eAddr;
        };
        // https://forums.pcsx2.net/archive/index.php/thread-13784-3.html
        static std::array<CoreHoles, 1> haltRegions{
            // (BIFC0) Speed Hack
            {{0x81fc0, 0x81fe0}}
        };
        ranges::for_each(haltRegions, [&](auto& region) {
            if (cpu->eePc >= region.iAddr && cpu->eePc < region.eAddr)
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