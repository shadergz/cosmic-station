#pragma once

#include <common/types.h>
#include <os/neon_simd.h>
#include <os/mapped.h>
namespace cosmic::gs {
    struct GsPayloadDataPacket {
        u32 qw128Count;
        os::MappedMemory<os::vec> downloadBuffer;
        u32 indexAddr;

        os::vec consume() {
            os::vec data{downloadBuffer[indexAddr]};
            indexAddr++;
            qw128Count--;
            return data;
        }
    };
    class GsEngine {
    public:
        void resetGraphics();
        std::tuple<bool, os::vec> readGsData();
    private:
        GsPayloadDataPacket transferBuffer;
    };
}
