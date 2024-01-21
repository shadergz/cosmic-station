#include <common/except.h>
#include <engine/copctrl/cop0.h>
#include <engine/ee_core.h>

namespace cosmic::engine::copctrl {
    // We don't check for a cache miss here
    os::vec CtrlCop::readCache(u32 address, CacheMode mode) {
        u32 tag{getCachePfn(address, mode)};
        Ref<CopCacheLine> cache;
        cache = getCache(address, false, mode);
        u8 fix{};
        if (cache->tags[0] == tag)
            fix = 1;
        else if (cache->tags[1] == tag)
            fix = 2;

        if (!fix) {
            throw Cop0Fail("Address {} isn't cached or doesn't have a valid tag referencing it", address);
        }
        const CopCacheLine::CacheWay cont{cache->ec[fix - 1]};
        return cont.vec[address & 3];
    }
    void CtrlCop::invIndexed(u32 address) {
        auto cc{getCache(address, true)};
        cc->tags[0] |= ~validBit;
        cc->tags[1] |= ~validBit;
        cc->lrf[0] = cc->lrf[1] = {};

        std::memset(cc->ec.data(), 0, sizeof(cc->ec));
    }
    bool CtrlCop::isCacheHit(u32 address, u8 lane, CacheMode mode) {
        // Each cache line is indexed by virtual address
        u16 tag{getCachePfn(address, mode)};
        auto highway{getCache(address, false, mode)};

        if (!highway)
            return false;

        if (lane == 0 && highway->lrf[0])
            return highway->tags[0] == tag;
        if (lane == 1 && highway->lrf[1])
            return highway->tags[1] == tag;
        return {};
    }
    void CtrlCop::loadCacheLine(u32 address, EeMipsCore& core, CacheMode mode) {
        Ref<CopCacheLine> pear{};
        u16 logical{getCachePfn(address, mode)};

        pear = getCache(address, true, mode);
        assignFlushedCache(*pear, logical);
        if (pear->tags[0] != logical && pear->tags[1] != logical) {
            throw Cop0Fail("No portion of the cache line {} was properly selected! Tags: {}", logical,
                fmt::join(pear->tags, ","));
        }
        auto cacheData{core.mipsRead<os::vec>(address)};
        // Due to the LRF algorithm, we will write to the way that was written last (thus keeping
        // the last data among the ways in the cache, waiting for one more miss)
        u8 way;
        way = pear->lrf[0] && !pear->lrf[1];
        if (!way) {
            if (!pear->lrf[0] && pear->lrf[1])
                way = 2;
        }
        if (!way)
            way = 255;
        u8 missPenalty{40};

        switch (way) {
        case 0xff:
            pear->ec[0].vec[0] = cacheData;
            pear->ec[1].vec[1] = core.mipsRead<os::vec>(address + 64);
            pear->ec[1].vec[2] = core.mipsRead<os::vec>(address + 64 * 2);
            pear->ec[1].vec[3] = core.mipsRead<os::vec>(address + 64 * 3);
            missPenalty *= 4;
            break;
        case 1 ... 2:
            pear->ec[way - 1].vec[address & 3] = cacheData;
            missPenalty *= 2;
            break;
        }
        core.runCycles -= missPenalty;
    }
    u16 CtrlCop::getCachePfn(u32 address, CacheMode mode) {
        if (mode == Instruction)
            return static_cast<u16>(address >> 13);
        else
            return static_cast<u16>(address >> 12);
    }

    void CtrlCop::assignFlushedCache(CopCacheLine& eec, u32 tag, CacheMode mode) {
        // The EE uses a Least Recently Filled (LRF) algorithm to determine which way to load data into
        u32 assign{};
        std::array<u8, 2> mix{};
        mix[0] = static_cast<u8>(eec.tags[0] & ~validBit);
        mix[1] = static_cast<u8>(eec.tags[1] & ~validBit);

        if (mix[0] && !mix[1])
            assign = 0;
        if (mix[1] && !mix[0])
            assign = 1;

        eec.lrf[0] ^= true;
        eec.lrf[1] ^= true;
        if (!assign) {
            // The row to fill is the XOR of the LFU bits
            assign = (eec.lrf[0] ^ eec.lrf[1]);
            eec.lrf[assign] ^= true;
        }
        // Here is where we write the tag bits; we expect all bits to be equal to 0
        eec.tags[assign] &= 0xffffffff | tag;
        switch (mode) {
        case Instruction:
            if (eec.tags[assign] & validBit)
                ;
            eec.tags[assign] |= validBit;
            break;
        case Data:
            if (eec.tags[assign] & dirtyBit)
                ;
            eec.tags[assign] |= dirtyBit;
        }
    }
    Ref<CopCacheLine> CtrlCop::getCache(u32 mem, bool write, CacheMode mode) {
        std::array<u8*, 2> wb;
        std::array<bool, 2> valid;
        u32 ci;
        std::span<CopCacheLine> cc;
        if (mode == Instruction) {
            ci = (mem >> 6) & 0x7f;
            cc = inCache;
        } else {
            ci = (mem >> 6) & 0x3f;
            cc = dataCache;
        }
        wb[0] = virtMap[cc[ci].tags[0] >> 12];
        valid[0] = cc[ci].lrf[0];
        valid[1] = cc[ci].lrf[1];
        wb[1] = virtMap[cc[ci].tags[1] >> 12];

        if (wb[0] == virtMap[mem >> 12] && valid[0])
            return cc[ci];
        if (wb[1] == virtMap[mem >> 12] && valid[1])
            return cc[ci];
        u32 way{((cc[ci].tags[0] >> 6) & 1) ^ ((cc[ci].tags[1] >> 6) & 1)};
        const auto isDirty{static_cast<bool>(cc[ci].tags[way] & dirtyBit)};

        if (write && mode == Data && isDirty) {
            auto wrm{wb[way] + (mem & 0xfc0)};
            BitCast<u64*>(wrm)[0] = cc[ci].ec[way].large[0];
            BitCast<u64*>(wrm)[1] = cc[ci].ec[way].large[1];
            BitCast<u64*>(wrm)[2] = cc[ci].ec[way].large[2];
            BitCast<u64*>(wrm)[3] = cc[ci].ec[way].large[3];
            BitCast<u64*>(wrm)[4] = cc[ci].ec[way].large[4];
            BitCast<u64*>(wrm)[5] = cc[ci].ec[way].large[5];
            BitCast<u64*>(wrm)[6] = cc[ci].ec[way].large[6];
            BitCast<u64*>(wrm)[7] = cc[ci].ec[way].large[7];
        }
        if (write)
            // If we are writing to the cache, the dirty bit must be set
            cc[ci].tags[way] |= ~dirtyBit;
        return cc[ci];
    }
}
