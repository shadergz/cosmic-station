#pragma once

#include <vector>
#include <common/types.h>
#include <os/neon_simd.h>
namespace cosmic::vu {
    enum FifoMethodVif {
        // Clean the valid bit, and if (gsValue == 0x2), clean gsData
        FifoClean,
        // Set a valid value
        FifoSet,
        // Return the value at the index
        FifoLoad
    };

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
        void resetVFifo();
        u32 size() const {
            return fifoInd;
        }
        void pushQuad(os::vec& gsd);
        u32 push(u32 gsValue);
        inline u32 update(u32 index, u32 gsValue, FifoMethodVif fifoMode);
        u32 fifoInd;
        std::vector<VifDataPack> dataPack;
    };
}
