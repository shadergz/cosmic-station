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
            auto data{downloadBuffer[indexAddr]};
            indexAddr++;
            qw128Count--;
            return data;
        }
    };
    enum GsRegisters {
        GsBusDir
    };

    class GsEngine {
    public:
        void resetGraphics();
        std::tuple<bool, os::vec> readGsData();
        bool isStalled();
        u32 privileged(GsRegisters gsr);

        struct {
            // Must be set appropriately for GIF->VRAM and VRAM->GIF
            u8 busDir;
        } gsPrivateRegs;
    private:
        GsPayloadDataPacket transferBuffer;
    };
}
