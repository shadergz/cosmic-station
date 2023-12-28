#include <common/except.h>
#include <engine/copctrl/cop0.h>
#include <engine/ee_core.h>

namespace cosmic::engine::copctrl {
    // TODO: Cache indexes use PFN (Physical Frame Number) addressing; we need to redo this entire method
    void CoProcessor0::invIndexed(u32 address) {
        auto line{viewLine(address)};
        line->tags[address & 1] |= static_cast<u32>(1 << 31);
    }
    raw_reference<CopCacheLine> CoProcessor0::viewLine(u32 address) {
        u8 index{static_cast<u8>(address >> 6 & 0x7f)};
        return iCacheLines[index];
    }
    // We don't check for a cache miss here
    u32 CoProcessor0::readCache(u32 address) {
        u32 tag{address >> 13};
        auto line{viewLine(address)};
        u32 data;
        if (line->tags[0] == tag) {
            data = line->data[0];
        } else if (line->tags[1] == tag) {
            data = line->data[1];
        } else {
            throw Cop0Fail("Address {} isn't cached or doesn't have a valid tag referencing it", address);
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
    void CoProcessor0::loadCacheLine(u32 address, raw_reference<EeMipsCore> eeCore) {
        auto line{viewLine(address)};
        auto logical{address >> 13};
        fillCacheWay(line, logical);
        if (line->tags[0] != logical && line->tags[1] != logical) {
            throw Cop0Fail("No portion of the cache line {} was properly selected! tags[0]: {}, tags[1]: {}", logical, line->tags[0], line->tags[1]);
        }
        auto cacheData{eeCore->mipsRead<os::vec128>(address)};
        // Due to the LRF algorithm, we will write to the way that was written last (thus keeping
        // the last data among the ways in the cache, waiting for one more miss)
        if (line->lrf[0] && !line->lrf[1]) {
            line->data[0] = cacheData.to32(0);
        } else if (!line->lrf[0] && line->lrf[1]) {
            line->data[1] = cacheData.to32(0);
        } else {
            *bit_cast<u64*>(line->data) = cacheData.to64(0);
        }
        eeCore->runCycles -= 40;
    }

    void CoProcessor0::fillCacheWay(raw_reference<CopCacheLine> line, u32 tag) {
        // The EE uses a Least Recently Filled (LRF) algorithm to determine which way to load data into.
        [[unlikely]] if (line->tags[0] & invCacheBit) {
            line->lrf[0] ^= true;
            line->tags[0] = tag;
        } else [[likely]] if (line->tags[1] & invCacheBit) {
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
