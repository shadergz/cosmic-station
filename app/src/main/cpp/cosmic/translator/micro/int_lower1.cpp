#include <translator/micro/vum_code.h>
#include <vu/vecu.h>

namespace cosmic::translator::micro {
    void VuMicroInterpreter::iddai(VuMicroOperands& ops) {
        vuMicro->writeInt(ops.bc & 0xf, ops.src & 0xf);
        i16 imm{static_cast<i16>((ops.inst >> 6) & 0x1f)};
        imm = static_cast<i16>((imm & 0x10 ? 0xfff0 : 0) | (imm & 0xf));

        vu::VuIntReg vui{vuMicro->intsRegs[ops.src].sig + imm};
        vuMicro->intsRegs[ops.bc] = vui;
    }
}