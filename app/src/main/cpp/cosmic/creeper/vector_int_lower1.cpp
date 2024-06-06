#include <creeper/vector_codes.h>
#include <vu/vecu.h>

namespace cosmic::creeper {
    void VuMicroInterpreter::iddai(VuMicroOperands& ops) {
        vu->pushIntPipe(ops.ft & 0xf, ops.fs & 0xf);
        i16 imm{static_cast<i16>((ops.inst >> 6) & 0x1f)};
        imm = static_cast<i16>((imm & 0x10 ? 0xfff0 : 0) | (imm & 0xf));

        vu::VuIntReg vui{vu->intsRegs[ops.fs].sig + imm};
        vu->intsRegs[ops.ft] = vui;
    }
    void VuMicroInterpreter::mtir(VuMicroOperands& ops) {
        vu->pushIntPipe(ops.ft & 0xf, 0);
        u32 id{(ops.inst >> 21) & 0x3};
        if (id > 3) {
        }
        vu->intsRegs[ops.ft & 0xf].uns = static_cast<u16>(vu->VuGPRs[ops.fs].uns[id]);
    }
}