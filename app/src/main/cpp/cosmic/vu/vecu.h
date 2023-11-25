#pragma once

#include <common/types.h>
#include <os/neon_simd.h>
namespace cosmic::vu {
    struct VUWorkMemory {
        std::array<u8, 1024 * 16> heap;
    };
    union alignas(16) VUReg {
        f32 floats[4];
        u32 uns[4];
        i32 sig[4];
        os::machVec128 faster{};
    };
    union alignas(2) VUIntReg {
        i16 sig;
        u16 uns;
    };
    class VectorUnit {
    public:
        VectorUnit() = default;

        void resetVU();
        void softwareReset();

        void pulse(u32 cycles);
    private:
        u16 status;
        bool isVUBusy, isOnBranch{false};
            // Each pipeline is specialized in a certain domain
        u64 pipeStates[2];

        std::array<u8, 4> clipFlags;
        std::array<u16, 4> macFlags;

        VUWorkMemory vuSpace;
        u32 vuPC{};
        alignas(512) VUReg VuGPRs[32];
        alignas(32) VUIntReg intsRegs[16];
    };
}
