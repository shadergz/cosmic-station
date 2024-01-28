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
        case PrimitiveOffset:
            gs->gsWrite(0x00, packet[0]);
            break;
        case RGBAQOffset: {
            RGBAQReg color;
            color.r = colorUnzip(packet[0], 0 );
            color.g = colorUnzip(packet[0], 32);
            color.b = colorUnzip(packet[1], 0 );
            color.a = colorUnzip(packet[1], 32);
            color.gsq = gsQ;

            gs->gsWrite(0x01, color.rainbow);
        }
            break;
        case StPosOffset: {
            u64 neoQ;
            // Fixing float types, this can be remedied or disabled later...
            neoQ = packet[1] & 0x7f800000;
            if ((neoQ & 0x7f800000) == 0x7f800000)
                neoQ = (neoQ & 0x80000000) | 0x7f7fffff;
            gs->gsWrite(0x02, packet[0]);

            gsQ = *reinterpret_cast<f32*>(&neoQ);
        }
            break;
        case UvPosOffset:
            gs->gsWrite(0x03, packet[0]);
            break;
        case Xyz2Offset: {
            CoordinatesXyz c;
            c.x = packet[0] & 0xffff;
            c.y = (packet[0] >> 32) & 0xffff;
            c.z = packet[1] & 0xffffffff;
            gs->gsWrite(0x05, c.xyz);
        }
        case NopOffset:
            break;
        case FogOffset ... AdOffset: {
            u32 addr{static_cast<u32>(packet[1] & 0xff)};
            if (addr < 0x7f) {
                gs->gsWrite(addr, packet[0]);
            }
        }
            break;
        default:
            gs->gsWrite(reg, packet[0]);
            break;
        }
    }
}