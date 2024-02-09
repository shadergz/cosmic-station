#include <creeper/micro/vum_code.h>
#include <vu/vecu.h>
namespace cosmic::creeper::micro {
    void VuMicroInterpreter::mr32(VuMicroOperands& ops) {
        static const std::array<u8, 4> rotates{8, 4, 2, 1};
        f32 fsX{vuMicro->VuGPRs[ops.fs].floats[0]};

        const i32 x{ops.dest & rotates[0]};
        const i32 y{ops.dest & rotates[1]};
        const i32 z{ops.dest & rotates[2]};
        const i32 w{ops.dest & rotates[3]};

        if (x)
            vuMicro->VuGPRs[ops.ft].floats[0] = vuMicro->VuGPRs[ops.fs].floats[1];
        if (y)
            vuMicro->VuGPRs[ops.ft].floats[1] = vuMicro->VuGPRs[ops.fs].floats[2];
        if (z)
            vuMicro->VuGPRs[ops.ft].floats[2] = vuMicro->VuGPRs[ops.fs].floats[3];
        if (w)
            vuMicro->VuGPRs[ops.ft].floats[3] = fsX;
    }
}

