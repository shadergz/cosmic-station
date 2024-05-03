#include <range/v3/algorithm.hpp>
#include <vu/vif_fifo.h>
namespace cosmic::vu {
    inline u32 VifFifo::update(u32 index, u32 gsValue, FifoMethodVif fifoMode) {
        std::vector<VifDataPack>::iterator bg{std::begin(dataPack)};
        u32 distance{};
        for (; bg != std::end(dataPack); bg++) {
            distance = static_cast<u32>(std::distance(bg, std::end(dataPack)));
            [[unlikely]] if (fifoMode == FifoClean) {
                if (index != distance)
                    continue;
                bg->gsData = gsValue;
                bg->isValid = {};
            } else if (fifoMode == FifoSet) {
                bg->gsData = gsValue;
                bg->isValid = true;
            } else {
                return bg->gsData;
            }
        }
        return distance;
    }
    u32 VifFifo::push(u32 gsValue) {
        return update(fifoInd++, gsValue, FifoSet);
    }
    void VifFifo::resetVFifo() {
        for (auto& gsWord: dataPack) {
            gsWord.isValid = false;
        }
        fifoInd = {};
    }
    void VifFifo::pushQuad(os::vec& gsd) {
        for (u8 vl{}; vl < 3; vl++)
            push(gsd.to32(vl));
    }
    VifFifo::VifFifo(u32 queueSize) {
        dataPack.resize(queueSize);
        // resetVFifo();
        fifoInd = {};
    }
}
