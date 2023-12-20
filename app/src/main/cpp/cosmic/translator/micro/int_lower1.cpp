#include <translator/micro/vum_code.h>
#include <vu/vecu.h>

namespace cosmic::translator::micro {
    void VuMicroInterpreter::iddai(VuMicroOperands& ops) {
        vuMicro->pushIntPipe(ops.ft & 0xf, ops.fs & 0xf);
        i16 imm{static_cast<i16>((ops.inst >> 6) & 0x1f)};
        imm = static_cast<i16>((imm & 0x10 ? 0xfff0 : 0) | (imm & 0xf));

        vu::VuIntReg vui{vuMicro->intsRegs[ops.fs].sig + imm};
        vuMicro->intsRegs[ops.ft] = vui;
    }
    void VuMicroInterpreter::mr32(VuMicroOperands& ops) {
        static const u8 rotates[]{8, 4, 2, 1};
        f32 fsXBackup{vuMicro->VuGPRs[ops.fs].floats[0]};

        const i32 _x{ops.dest & rotates[0]};
        const i32 _y{ops.dest & rotates[1]};
        const i32 _z{ops.dest & rotates[2]};
        const i32 _w{ops.dest & rotates[3]};

        if (_x)
            vuMicro->VuGPRs[ops.ft].floats[0] = vuMicro->VuGPRs[ops.fs].floats[1];
        if (_y)
            vuMicro->VuGPRs[ops.ft].floats[1] = vuMicro->VuGPRs[ops.fs].floats[2];
        if (_z)
            vuMicro->VuGPRs[ops.ft].floats[2] = vuMicro->VuGPRs[ops.fs].floats[3];
        if (_w)
            vuMicro->VuGPRs[ops.ft].floats[3] = fsXBackup;
    }
    void VuMicroInterpreter::mtir(VuMicroOperands& ops) {
        vuMicro->pushIntPipe(ops.ft & 0xf, 0);
        u32 id{(ops.inst >> 21) & 0x3};
        if (id > 3)
            ;
        vuMicro->intsRegs[ops.ft & 0xf].uns = static_cast<u16>(vuMicro->VuGPRs[ops.fs].uns[id]);
    }
}