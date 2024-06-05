#include <range/v3/algorithm.hpp>
#include <vu/vif_fifo.h>
namespace cosmic::vu {
    inline u32 VifFifo::accessData(u32 index, u32 gsValue, FifoMethodVif fifoMode) {
        if (index >= dataPack.size()) {
            return {};
        }
        auto& dataAtRange{dataPack.at(index)};
        if (fifoMode != FifoLoad)
            dataAtRange.gsData = gsValue;

        if (fifoMode == FifoClean)
            dataAtRange.isValid = {};
        if (fifoMode == FifoSet)
            dataAtRange.isValid = true;

        if (fifoMode == FifoLoad)
            return dataAtRange.gsData;
        return {};
    }
    void VifFifo::push(u32 gsValue) {
        accessData(fifoIndex++, gsValue, FifoSet);
    }
    u32 VifFifo::consume() {
        u32 front{};
        front = accessData(static_cast<u32>(--fifoIndex), 0, FifoLoad);
        if (dataPack.size()) {
            dataPack.at(fifoIndex).isValid = {};
        }
        return front;
    }
    void VifFifo::resetVifFifo() {
        if (!dataPack.size())
            dataPack.resize(64);

        for (auto& packet: dataPack) {
            packet.isValid = {};
        }
        fifoIndex = {};
    }
    void VifFifo::pushQuad(os::vec& gsd) {
        for (u8 vl{}; vl < 3; vl++) {
            push(gsd.to32(vl));
        }
    }
    VifFifo::VifFifo(u32 queueSize) {
        dataPack.resize(queueSize);
        // resetVifFifo();
        fifoIndex = {};
    }
}
