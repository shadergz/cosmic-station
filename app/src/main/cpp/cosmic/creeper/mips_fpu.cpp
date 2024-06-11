// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <creeper/cached_blocks.h>
#include <common/global.h>

#define DSP_MATH_PARAMS 1
namespace cosmic::creeper {
    std::array<f32, 3> fpList;
    u8 dest;

    void MipsIvInterpreter::fpuMadd(Operands ops) {
        fpList[0] = fpu->sony754con(fpu->fprRegs[ops.rd].un);
        fpList[1] = fpu->sony754con(fpu->fprRegs[ops.rt].un);
        fpList[2] = fpu->sony754con(fpu->acc.un);
#if DSP_MATH_PARAMS
        user->info("(Creeper, FPU): Converted result to Sony 754 values: {}", fmt::join(fpList, "/ "));
#endif
        dest = static_cast<u8>((ops.inst >> 6) & 0x1f);

        fpu->fprRegs[dest].decimal = fpList[2] + (fpList[0] * fpList[1]);
        fpu->checkOverflow(dest);
        fpu->checkUnderflow(dest);
    }
    void MipsIvInterpreter::fpuAdda(Operands ops) {
        fpList[0] = fpu->sony754con(fpu->fprRegs[ops.rd].un);
        fpList[1] = fpu->sony754con(fpu->fprRegs[ops.rt].un);

        fpu->acc.decimal = fpList[0] + fpList[1];
        fpu->checkOverflow(32);
        fpu->checkUnderflow(32);
    }
}