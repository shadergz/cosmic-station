#include <gs/gif_bridge.h>
#include <gs/synth_engine.h>
namespace cosmic::gs {

    void GifBridge::uploadPackedData(Ref<GifTag>& dsTag, u64 packet[2]) {
        RegDesc reg{};
        u64 offset;

        if (!dsTag->isEndOfPacket) {
            offset = (dsTag->regsNum - dsTag->leftRegsData[0]) << 2;
            if (offset > 63) {

            }
            reg = static_cast<RegDesc>((dsTag->regs >> offset) & 0xf);
        }

        switch (reg) {
        case RegDesc::Primitive:
            gs->gsWrite(0x00, packet[0]);
            break;
        case RegDesc::RGBAQ: {
            RGBAQReg color{};
            // NOTES: There was a mistake in the type of bitwise operation
            // used to extract the values below
            color.r = extractPair<u8>(packet[0], 0, 0xff);
            color.g = extractPair<u8>(packet[0], 32, 0xff);

            color.b = extractPair<u8>(packet[1], 0, 0xff);
            color.a = extractPair<u8>(packet[1], 32, 0xff);
            // The internal Q register is used here and stays the same
            color.gsq = gsQ;

            gs->gsWrite(0x01, color.rainbow);
        }
            break;
        case RegDesc::StPos: {
            u64 neoQ;
            // Fixing float types, this can be remedied or disabled later...
            neoQ = extractPair<u64>(packet[1], 0, 0x7f800000);
            if ((neoQ & 0x7f800000) == 0x7f800000)
                neoQ = (neoQ & 0x80000000) | 0x7f7fffff;
            gs->gsWrite(0x02, packet[0]);

            gsQ = *reinterpret_cast<f32*>(&neoQ);
        }
            break;
        case RegDesc::UvPos: {
            std::array<u16, 2> uvsCods{};
            uvsCods[0] = extractPair<u16>(packet[0], 0, 0x3fff);
            uvsCods[1] = extractPair<u16>(packet[0], 32, 0x3fff);
            gs->gsWrite(0x03, *reinterpret_cast<u32 *>(uvsCods.data()));
        }
            break;
        case RegDesc::Xyz2: {
            CoordinatesXyz c{
                .x = extractPair<u16>(packet[0], 0, 0xffff),
                .y = extractPair<u16>(packet[0], 32, 0xffff),
                .z = extractPair<u32>(packet[1], 0, 0xffffffff)
            };
            auto disableDraw{(packet[1] >> (111 - 64)) & 1};
            auto address{disableDraw ? 0xd : 0x5};

            gs->gsWrite(static_cast<u32>(address), c.xyz);
        }
        case RegDesc::Nop:
            break;
        case RegDesc::Fog ... RegDesc::Ad: {
            u32 addr{static_cast<u32>(packet[1] & 0xff)};
            if (addr > 0x7f) {

            }
            gs->gsWrite(addr, packet[0]);
        }
            break;
        default:
            gs->gsWrite(reg, packet[0]);
            break;
        }
    }
}