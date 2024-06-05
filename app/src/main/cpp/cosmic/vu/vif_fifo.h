#pragma once

#include <vector>
#include <common/types.h>
#include <os/neon_simd.h>
namespace cosmic::vu {
    enum FifoMethodVif {
        // Clear the valid bit and delete the stored value at the specified index
        FifoClean,
        // Set a valid value
        FifoSet,
        // Return the value at the index
        FifoLoad
    };

    // A vector-based FIFO - We will not delete our FIFO data, just mark it as trash
    // This will satisfy the need to not reallocate queue data during execution
    struct VifDataPack {
        u32 gsData;
        bool isValid;
    };
    class VifFifo {
    public:
        VifFifo() = default;
        VifFifo(u32 queueSize);
        void resetVifFifo();
        u32 size() const {
            return fifoIndex;
        }
        void pushQuad(os::vec& gsd);
        void push(u32 gsValue);
        u32 consume();

        u32 accessData(u32 index, u32 gsValue, FifoMethodVif fifoMode);
        u32 fifoIndex;
        std::vector<VifDataPack> dataPack;
    };
}
