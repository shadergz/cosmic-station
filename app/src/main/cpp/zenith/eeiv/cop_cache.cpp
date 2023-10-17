#include <eeiv/ee_engine.h>

namespace zenith::eeiv {
    EECacheLine* CoProcessor0::viewLine(u32 address) {
        u8 index{static_cast<u8>(address >> 6 & 0x7f)};
        return &eeNearCache[index];
    }
    u32 CoProcessor0::readCache32(u32 address) {
        u8 offset{static_cast<u8>(address & 0x1f)};
        auto line{viewLine(address)};
        u32 data{};
        if (line->lrf[0]) {
            data = line->data[0] & (0x1f & offset);
        } else if (line->lrf[1]) {
            data = line->data[1] & (0x1f & offset);
        }
        return data;
    }

    bool CoProcessor0::isCacheHit(u32 address, u8 lane) {
        // Each cache line is indexed by virtual address
        u16 tag{static_cast<u16>(address >> 13)};
        auto line{viewLine(address)};
        return lane == 0 ?
            line->tags[0] == tag :
            line->tags[1] == tag;
    }

    void CoProcessor0::loadCacheLine(u32 address, EEMipsCore& eeCore) {
        auto line{viewLine(address)};
        auto cacheData{eeCore.tableRead<os::machVec128>(address)};
        if (!line->lrf[0] && line->lrf[1]) {
            line->data[0] = cacheData.to32(0);
        } else if (line->lrf[0] && !line->lrf[1]) {
            line->data[1] = cacheData.to32(0);
        } else {
            *reinterpret_cast<u64*>(line->data) = cacheData.to64(0);
        }
        eeCore.cyclesToWaste -= 40;
    }

    void CoProcessor0::fillCacheWay(u32 address, u32 tag) {
        // The EE uses a Least Recently Filled (LRF) algorithm to determine which way to load data into.
        // If either way does not have the Valid bit set, the EE sets the Valid bit in the first way
        // that does not have it and loads data into it. If both ways are full, the EE takes the XOR
        // of the LRF bits in the tags. The result is the index of the way it uses. The EE then flips
        // the LRF bit in the selected way.
        auto line{viewLine(address)};
        [[unlikely]] if (line->tags[0] & invCacheLRF) {
            line->lrf[0] ^= true;
            line->tags[0] = tag;
        } else [[likely]] if (line->tags[1] & invCacheLRF) {
            line->lrf[1] ^= true;
            line->tags[1] = tag;
        } else {
            // The row to fill is the XOR of the LFU bits
            auto way{line->lrf[0] ^ line->lrf[1]};
            line->tags[static_cast<u32>(way)] ^= true;
            line->tags[static_cast<u32>(way)] = tag;
        }
    }
}