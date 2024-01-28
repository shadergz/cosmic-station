#include <range/v3/algorithm.hpp>
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
    void GsEngine::gsWrite(u32 addr, u64 data) {
        addr &= 0x7f;
        u64 a, b;

        switch (addr) {
        case 0x00:
            writePrimitive(data);
            break;
        case 0x01:
            palette.rainbow = data;
            break;
        case 0x02:
            a = data & 0xffffff00;
            b = (data >> 32) & 0xffffff00;

            if ((a & 0x7f800000) == 0x7f800000)
                a = (a & 0x80000000) | 0x7f7fffff;
            if ((b & 0x7f800000) == 0x7f800000)
                b = (b & 0x80000000) | 0x7f7fffff;

            st = std::make_pair(
                *reinterpret_cast<f32*>(&a),
                *reinterpret_cast<f32*>(&b));
            break;
        case 0x03:
            uv = std::make_pair(data & 0x3fff, (data >> 16) & 0x3fff);
            break;
        case 0x05:
            xyz2.xyz = data;
            break;
        case 0x0a:
            fog = (data >> 56) & 0xff;
        case 0xf:
            break;
        default:
            // For some reason, the title Ridge Racer V uses the value 11 as a alias for the value 1
            ranges::for_each(gswAddrAlias, [&](auto& path) {
                if (path.gameCase == addr) {
                    gsWrite(path.rCase, data);
                }
            });
            break;
        }

    }
    void GsEngine::writePrimitive(u64 primitive) {
        [[likely]] if (!isSoftwareMode) {
            this->prim = primitive;
        }
    }
}
