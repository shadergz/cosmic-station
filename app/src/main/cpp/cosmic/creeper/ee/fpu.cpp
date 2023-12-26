// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <creeper/ee/mipsiv_cached.h>
#include <engine/ee_core.h>
namespace cosmic::creeper::ee {
    void MipsIvInterpreter::fpuMadd(Operands ops) {
        std::array<f32, 3> fo{};

        fo[0] = fpu->sony754con(fpu->fprRegs[ops.rd].un);
        fo[1] = fpu->sony754con(fpu->fprRegs[ops.rs].un);
        fo[2] = fpu->sony754con(fpu->acc.un);

        fpu->fprRegs[ops.rt].decimal = fo[2] + (fo[0] * fo[1]);
        fpu->checkOverflow(ops.rt);
        fpu->checkUnderflow(ops.rt);
    }
}