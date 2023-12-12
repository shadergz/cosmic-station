#pragma once

#include <vector>
#include <common/types.h>
#include <os/neon_simd.h>
namespace cosmic::vu {
    // A vector-based FIFO; we will not delete our FIFO data, just mark it as trash.
    // This will avoid vector data movement and FIFO allocation and de-allocation
    struct VifDataPack {
        u32 gsData;
        bool isValid;
    };
    class VifFifo {
    public:
        VifFifo() = default;
        VifFifo(u32 queueSize);
        void resetVfifo();
        u32 size() const {
            return marked;
        }
        void pushQuad(os::vec128& gsd);
        u32 push(u32 gsValue);
        inline u32 walk(u32 index, u32 gsValue, u8 method);
        u32 marked;
        std::vector<VifDataPack> dataPack;
    };
}
