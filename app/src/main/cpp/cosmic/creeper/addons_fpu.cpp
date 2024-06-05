// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)

#include <creeper/cached_blocks.h>
#include <engine/ee_core.h>
#include <common/global.h>

#define DSP_MATH_PARAMS 0
namespace cosmic::creeper {
    std::array<f32, 3> fpList;
    u8 dest;
#define CHK_OVER_OR_UNDER(d)\
    fpu->checkOverflow(d);\
    fpu->checkUnderflow(d)

    void MipsIvInterpreter::fpuMadd(Operands ops) {
        fpList[0] = fpu->sony754con(fpu->fprRegs[ops.rd].un);
        fpList[1] = fpu->sony754con(fpu->fprRegs[ops.rt].un);
        fpList[2] = fpu->sony754con(fpu->acc.un);
#if DSP_MATH_PARAMS
        user->info("(Creeper, FPU): Converted result to Sony 754 values: {}", fmt::join(fpList, "/ "));
#endif
        dest = static_cast<u8>((ops.inst >> 6) & 0x1f);

        fpu->fprRegs[dest].decimal = fpList[2] + (fpList[0] * fpList[1]);
        CHK_OVER_OR_UNDER(dest);
    }
    void MipsIvInterpreter::fpuAdda(Operands ops) {
        fpList[0] = fpu->sony754con(fpu->fprRegs[ops.rd].un);
        fpList[1] = fpu->sony754con(fpu->fprRegs[ops.rt].un);

        fpu->acc.decimal = fpList[0] + fpList[1];
        CHK_OVER_OR_UNDER(32);
    }
}