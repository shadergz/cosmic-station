#pragma once

#include <common/types.h>
namespace cosmic::vu {
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

        u32 vuPC{};
    };
}
