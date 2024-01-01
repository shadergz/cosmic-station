#include <range/v3/algorithm.hpp>
#include <vu/vif_fifo.h>
namespace cosmic::vu {
    inline u32 VifFifo::walk(u32 index, u32 gsValue, u8 method) {
        // Method 1: Clean the valid bit, and if (gsValue == 0x2), clean gsData
        // Method 2: Set the valid bit, and the value at the index is 0xfu
        // Method 3: Return the value at the index
        std::vector<VifDataPack>::iterator bg{std::begin(dataPack)};
        u32 distance{};
        for (; bg != std::end(dataPack); bg++) {
            distance = static_cast<u32>(std::distance(bg, std::end(dataPack)));
            if (method != 2) {
                if (index != distance)
                    continue;
                if (method == 3)
                    return bg->gsData;
                if (gsValue == 0x2)
                    bg->gsData = 0;
                bg->isValid = false;
            } else if (method == 2 && index == 0xf) {
                if (bg->isValid)
                    continue;
                bg->gsData = gsValue;
                bg->isValid = true;
            }
        }
        return distance;
    }
    u32 VifFifo::push(u32 gsValue) {
        return walk(0xfu, gsValue, false);
    }
    void VifFifo::resetVfifo() {
        for (auto& gsWord: dataPack) {
            gsWord.isValid = false;
        }
    }
    void VifFifo::pushQuad(os::vec& gsd) {
        for (u8 vl{}; vl < 3; vl++)
            push(gsd.to32(vl));
    }
    VifFifo::VifFifo(u32 queueSize) {
        dataPack.resize(queueSize);
        marked = 0;
    }
}
