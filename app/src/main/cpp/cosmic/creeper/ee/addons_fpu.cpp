// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <creeper/ee/cached_blocks.h>
#include <engine/ee_core.h>
#include <common/global.h>

#define DSP_MATH_PARAMS 1
namespace cosmic::creeper::ee {
    std::array<f32, 3> fo{};
    u8 dest{};
#define CHK_OVER_OR_UNDER(d)\
    fpu->checkOverflow(d);\
    fpu->checkUnderflow(d)

    void MipsIvInterpreter::fpuMadd(Operands ops) {
        fo[0] = fpu->sony754con(fpu->fprRegs[ops.rd].un);
        fo[1] = fpu->sony754con(fpu->fprRegs[ops.rt].un);
        fo[2] = fpu->sony754con(fpu->acc.un);
#if DSP_MATH_PARAMS
        user->info("(Creeper, FPU): Converted result to Sony 754 values: {}", fmt::join(fo, "` "));
#endif
        dest = static_cast<u8>((ops.inst >> 6) & 0x1f);

        fpu->fprRegs[dest].decimal = fo[2] + (fo[0] * fo[1]);
        CHK_OVER_OR_UNDER(dest);
    }
    void MipsIvInterpreter::fpuAdda(Operands ops) {
        fo[0] = fpu->sony754con(fpu->fprRegs[ops.rd].un);
        fo[1] = fpu->sony754con(fpu->fprRegs[ops.rt].un);

        fpu->acc.decimal = fo[0] + fo[1];
        CHK_OVER_OR_UNDER(32);
    }
}