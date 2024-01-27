#include <gs/synth_engine.h>

namespace cosmic::gs {
    constexpr u64 downBufferSize{2048 * 2048 / 4};
    void GsEngine::resetGraphics() {
        transferBuffer.qw128Count = 0;
        transferBuffer.indexAddr = 0;
        if (!*transferBuffer.downloadBuffer)
            transferBuffer.downloadBuffer = os::MappedMemory<os::vec>{downBufferSize};
    }
    std::tuple<bool, os::vec> GsEngine::readGsData() {
        bool hasData{transferBuffer.qw128Count != 0};
        os::vec vec{};
        if (hasData) {
            const os::vec eve{transferBuffer.consume()};
            vec[0] = eve[0];
            vec[1] = eve[1];
        }
        return std::make_tuple(hasData, vec);
    }

    bool GsEngine::isStalled() {
        return {};
    }
    u32 GsEngine::privileged(GsRegisters gsr) {
        if (gsr == GsBusDir) {
            return gsPrivateRegs.busDir;
        }
        return {};
    }
}
