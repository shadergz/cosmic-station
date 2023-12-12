#include <gs/synth_engine.h>

namespace cosmic::gs {
    constexpr u64 downBufferSize{2048 * 2048 / 4};
    void GsEngine::resetGraphics() {
        transferBuffer.qw128Count = 0;
        transferBuffer.indexAddr = 0;
        if (!*transferBuffer.downloadBuffer)
            transferBuffer.downloadBuffer = os::MappedMemory<os::vec128>{downBufferSize};
    }
    std::tuple<bool, os::vec128> GsEngine::readGsData() {
        bool hasData{transferBuffer.qw128Count != 0};
        os::vec128 vec{};
        if (hasData) {
            const os::vec128 eve{transferBuffer.consume()};
            vec[0] = eve[0];
            vec[1] = eve[1];
        }
        return std::make_tuple(hasData, vec);
    }
}
