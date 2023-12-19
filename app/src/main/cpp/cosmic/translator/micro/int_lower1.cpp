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
    void VuMicroInterpreter::mr32(VuMicroOperands& ops) {
        static const u8 rotates[]{8, 4, 2, 1};
        f32 savedX{vuMicro->VuGPRs[ops.src].floats[0]};
        if (ops.field & rotates[0])
            vuMicro->VuGPRs[ops.bc].floats[0] = vuMicro->VuGPRs[ops.src].floats[1];
        if (ops.field & rotates[1])
            vuMicro->VuGPRs[ops.bc].floats[1] = vuMicro->VuGPRs[ops.src].floats[2];
        if (ops.field & rotates[2])
            vuMicro->VuGPRs[ops.bc].floats[2] = vuMicro->VuGPRs[ops.src].floats[3];
        if (ops.field & rotates[3])
            vuMicro->VuGPRs[ops.bc].floats[3] = savedX;
    }
}