#include <common/except.h>
#include <ee/cop0.h>
#include <ee/ee_core.h>

namespace cosmic::ee {
    // We don't check for a cache miss here
    os::vec CtrlCop::readCache(u32 address, CacheMode mode) {
        auto tagAddr{getCachePfn(address, mode)};
        auto cache{getCache(address, false, mode)};
        u8 fix{};
        tagAddr |= dirtyBit;
        if (cache->tags[0] == tagAddr)
            fix = 1;
        else if (cache->tags[1] == tagAddr)
            fix = 2;

        if (!fix) {
            throw Cop0Err("Address {} isn't cached or doesn't have a valid tag referencing it", address);
        }
        const auto& cont{cache->ec[fix - 1]};
        return cont.vec[(address >> 4) & 3];
    }
    void CtrlCop::invIndexed(u32 address) {
        auto cc{getCache(address, true)};
        cc->tags[0] &= ~dirtyBit;
        cc->tags[1] &= ~dirtyBit;
        cc->lrf[0] = cc->lrf[1] = {
        };

        std::memset(cc->ec.data(), 0, sizeof(cc->ec));
    }
    bool CtrlCop::isCacheHit(u32 address, u8 lane, CacheMode mode) {
        // Each cache line is indexed by virtual address
        auto addrTag{getCachePfn(address, mode)};
        auto highway{getCache(address, false, mode)};

        if (!highway)
            return {};
        addrTag |= dirtyBit;

        switch (lane) {
        case 0:
            return highway->tags[0] == addrTag;
        case 1:
            return highway->tags[1] == addrTag;
        case 2:
            return
                highway->tags[0] == addrTag ||
                highway->tags[1] == addrTag;
        }
        return {};
    }
    void CtrlCop::loadCacheLine(u32 address, EeMipsCore& core, CacheMode mode) {
        auto masterIndex{
            getCachePfn(address, mode)};
        auto pear{
            getCache(address, true, mode)};

        assignFlushedCache(*pear, masterIndex);
        masterIndex |= dirtyBit;

        if ((pear->tags[0] != masterIndex && pear->tags[1] != masterIndex)) {
            throw Cop0Err(
                "No portion of the cache line {:#x} was properly selected! Tags: {:#x}",
                masterIndex, fmt::join(pear->tags, ", "));
        }
        // Due to the LRF algorithm, we will write to the way that was written last (thus keeping
        // the last data among the ways in the cache, waiting for one more miss)
        u8 way{pear->lrf[0] && !pear->lrf[1]};
        if (!way) {
            if (!pear->lrf[0] && pear->lrf[1])
                way = 2;
        }
        if (!way)
            // Means that the entire cache line is available for writing
            way = 255;
        u8 missPenalty{40};

        switch (way) {
        case 0xff:
            pear->ec[1].vec[1] = core.mipsRead<os::vec>((address + 64));
            pear->ec[1].vec[1] = core.mipsRead<os::vec>((address + 64) + 16);
            pear->ec[1].vec[2] = core.mipsRead<os::vec>((address + 64) + 16 * 2);
            pear->ec[1].vec[3] = core.mipsRead<os::vec>((address + 64) + 16 * 3);
            missPenalty *= 4;
            way = 1;
        case 1 ... 2:
            pear->ec[way - 1].vec[0] = core.mipsRead<os::vec>(address + 0);
            pear->ec[way - 1].vec[1] = core.mipsRead<os::vec>(address + 16);
            pear->ec[way - 1].vec[2] = core.mipsRead<os::vec>(address + 16 * 2);
            pear->ec[way - 1].vec[3] = core.mipsRead<os::vec>(address + 16 * 3);

            if (way != 0xff)
                missPenalty *= 2;
            break;
        }
        core.runCycles -= missPenalty;
    }
    u32 CtrlCop::getCachePfn(u32 address, CacheMode mode) {
        if (mode == Instruction)
            return static_cast<u16>(address >> 13);
        else
            return static_cast<u16>(address >> 12);
    }

    void CtrlCop::assignFlushedCache(CopCacheLine& eec, u32 tag, CacheMode mode) {
        // The EE uses a Least Recently Filled (LRF) algorithm to
        // determine which way to load data into
        u32 assign{};
        const std::array<u32, 2> mix{
            eec.tags[0] & dirtyBit,
            eec.tags[1] & dirtyBit
        };

        if (mix[0] && !mix[1]) assign = 1;
        if (mix[1] && !mix[0]) assign = 2;

        if (assign) {
            assign--;
            eec.lrf[assign] = true;
        } else {
            // The row to fill is the XOR of the LFU bits
            assign = (eec.lrf[0] ^ eec.lrf[1]);
            eec.lrf[assign] = true;
        }
        // Here is where we write the tag bits
        eec.tags[assign] = tag | dirtyBit;
    }
    Ref<CopCacheLine> CtrlCop::getCache(u32 mem, bool write, CacheMode mode) {
        std::array<Ref<u8*>, 2> wb;
        u32 ci;
        std::span<CopCacheLine> cc;
        if (mode == Instruction) {
            ci = (mem >> 6) & 0x7f;
            cc = inCache;
        } else {
            ci = (mem >> 6) & 0x3f;
            cc = dataCache;
        }
        wb[0] = Ref(virtMap[cc[ci].tags[0] >> 12]);
        std::array<bool, 2> valid{
            cc[ci].lrf[0],
            cc[ci].lrf[1]
        };
        wb[1] = Ref(virtMap[cc[ci].tags[1] >> 12]);

        if (*wb[0] == virtMap[mem >> 12] && valid[0])
            return cc[ci];
        if (*wb[1] == virtMap[mem >> 12] && valid[1])
            return cc[ci];
        const u32 way{(
            (cc[ci].tags[0] >> 6) & 1) ^ ((cc[ci].tags[1] >> 6) & 1)
        };
        const auto isDirty{static_cast<bool>(cc[ci].tags[way] & dirtyBit)};

        if (write && mode == Data && isDirty) {
            uintptr_t wrm{*(*wb[way]) + (mem & 0xfc0)};
            BitCast<u64*>(wrm)[0] = cc[ci].ec[way].large[0];
            BitCast<u64*>(wrm)[1] = cc[ci].ec[way].large[1];
            BitCast<u64*>(wrm)[2] = cc[ci].ec[way].large[2];
            BitCast<u64*>(wrm)[3] = cc[ci].ec[way].large[3];
            BitCast<u64*>(wrm)[4] = cc[ci].ec[way].large[4];
            BitCast<u64*>(wrm)[5] = cc[ci].ec[way].large[5];
            BitCast<u64*>(wrm)[6] = cc[ci].ec[way].large[6];
            BitCast<u64*>(wrm)[7] = cc[ci].ec[way].large[7];
        }
        if (write) {
            // If we are writing to the cache, the dirty bit must be set
            cc[ci].tags[way] |= dirtyBit;
        }
        return cc[ci];
    }
}
