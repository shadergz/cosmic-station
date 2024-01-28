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
    enum RegDesc {
        PrimitiveOffset,
        RGBAQOffset,
        StPosOffset,
        UvPosOffset,
        Xyz2Offset,

        FogOffset = 0xa,
        AdOffset = 0xe,

        NopOffset = 0xf
    };

    union RGBAQReg {
        u64 rainbow;
        struct {
            u8 r, g, b, a;
            float gsq;
        };
    };
    union CoordinatesXyz {
        u64 xyz;
        struct {
            u16 x, y;
            u32 z;
        };
    };

    class GsEngine {
    public:
        void resetGraphics();
        std::tuple<bool, os::vec> readGsData();
        bool isStalled();
        u32 privileged(GsRegisters gsr);

        bool isSoftwareMode{};
        void gsWrite(u32 addr, u64 data);

        struct {
            // Must be set appropriately for GIF->VRAM and VRAM->GIF
            u8 busDir;
        } gsPrivateRegs;
    private:
        GsPayloadDataPacket transferBuffer;

        // Internal registers (accessible via GIF)
        u64 prim;
        RGBAQReg palette{};
        std::pair<f32, f32> st;
        std::pair<u16, u16> uv;
        CoordinatesXyz xyz2;
        u8 fog;

        void writePrimitive(u64 primitive);
    };
}
