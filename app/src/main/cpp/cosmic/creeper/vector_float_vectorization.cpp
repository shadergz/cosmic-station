#include <creeper/vector_codes.h>
#include <vu/vecu.h>
namespace cosmic::creeper {
    void VuMicroInterpreter::mr32(VuMicroOperands& ops) {
        static const std::array<u8, 4> rotates{8, 4, 2, 1};
        f32 fsX{vu->VuGPRs[ops.fs].floats[0]};

        const i32 x{ops.fd & rotates[0]};
        const i32 y{ops.fd & rotates[1]};
        const i32 z{ops.fd & rotates[2]};
        const i32 w{ops.fd & rotates[3]};

        if (x)
            vu->VuGPRs[ops.ft].floats[0] = vu->VuGPRs[ops.fs].floats[1];
        if (y)
            vu->VuGPRs[ops.ft].floats[1] = vu->VuGPRs[ops.fs].floats[2];
        if (z)
            vu->VuGPRs[ops.ft].floats[2] = vu->VuGPRs[ops.fs].floats[3];
        if (w)
            vu->VuGPRs[ops.ft].floats[3] = fsX;
    }
}

