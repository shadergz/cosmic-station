#include <range/v3/algorithm.hpp>
#include <gs/gs_engine.h>

namespace cosmic::gs {
    constexpr u64 downBufferSize{2048 * 2048 / 4};
    void GsEngine::resetGraphics() {
        videoBuffer.qw128Count = {};
        videoBuffer.indexAddr = {};

        framesCount = {};
        if (!videoBuffer) {
            videoBuffer = GsPayloadDataPacket{downBufferSize};
        }
    }
    std::tuple<bool, os::vec> GsEngine::readGsData() {
        bool hasData{videoBuffer.qw128Count != 0};
        os::vec vec{};
        if (hasData) {
            const auto eve{videoBuffer.consume()};
            vec[0] = eve[0];
            vec[1] = eve[1];
        }
        return std::make_tuple(hasData, vec);
    }

    bool GsEngine::isStalled() {
        return {};
    }
    u32 GsEngine::privileged(GsRegisters gsr) const {
        if (gsr == GsBusDir) {
            return gsPrivateRegs.busDir;
        }
        return {};
    }
    void GsEngine::gsWrite(u32 addr, u64 data) {
        addr &= 0x7f;

        switch (addr) {
        case 0x00:
            writePrimitive(data);
            break;
        case 0x01:
            palette.rainbow = data;
            break;
        case 0x02: {
            auto fp0{data & 0xffffff00};
            auto fp1{(data >> 32) & 0xffffff00};

            if ((fp0 & 0x7f800000) == 0x7f800000)
                fp0 = (fp0 & 0x80000000) | 0x7f7fffff;
            if ((fp1 & 0x7f800000) == 0x7f800000)
                fp1 = (fp1 & 0x80000000) | 0x7f7fffff;

            st = std::make_pair(
                *reinterpret_cast<f32*>(&fp0), *reinterpret_cast<f32*>(&fp1));
        }
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
