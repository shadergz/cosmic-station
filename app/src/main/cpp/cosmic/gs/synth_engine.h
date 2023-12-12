#pragma once

#include <common/types.h>
#include <os/neon_simd.h>
#include <os/mapped.h>
namespace cosmic::gs {
    struct GsPayloadDataPacket {
        u32 qw128Count;
        os::MappedMemory<os::vec128> downloadBuffer;
        u32 indexAddr;

        os::vec128 consume() {
            os::vec128 data{downloadBuffer[indexAddr]};
            indexAddr++;
            qw128Count--;
            return data;
        }
    };
    class GsEngine {
    public:
        void resetGraphics();
        std::tuple<bool, os::vec128> readGsData();
    private:
        GsPayloadDataPacket transferBuffer;
    };
}
