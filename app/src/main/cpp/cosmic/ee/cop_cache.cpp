#include <common/except.h>
#include <ee/cop0.h>
#include <ee/ee_core.h>

namespace cosmic::ee {
    // We don't check for a cache miss here
    const os::vec& CtrlCop::readCache(u32 address, CacheMode mode) {
        auto tagAddr{getCachePfn(address, mode)};
        auto cachedData{getCache(address, false, mode)};
        u8 lineLayer{};
        tagAddr |= dirtyBit;
        if (cachedData.tags[0] == tagAddr)
            lineLayer = 1;
        else if (cachedData.tags[1] == tagAddr)
            lineLayer = 2;

        if (!lineLayer) {
            throw Cop0Err("Address {:#x} isn't cached or doesn't have a valid tag referencing it", address);
        }
        const auto& cont{cachedData.ways[lineLayer - 1]};
        return cont.vec[(address >> 4) & 3];
    }
    void CtrlCop::invIndexed(u32 address) {
        auto invWaysAt{getCache(address, true)};
        invWaysAt.tags[0] &= ~dirtyBit;
        invWaysAt.tags[1] &= ~dirtyBit;
        invWaysAt.lrf[0] = invWaysAt.lrf[1] = {
        };

        std::memset(invWaysAt.ways.data(), 0, sizeof(invWaysAt.ways));
    }
    bool CtrlCop::isCacheHit(u32 address, u8 lane, CacheMode mode) {
        // Each cache line is indexed by virtual address
        auto addrTag{getCachePfn(address, mode)};
        auto mirror{getCache(address, false, mode)};

        addrTag |= dirtyBit;

        switch (lane) {
        case 0:
            return mirror.tags[0] == addrTag;
        case 1:
            return mirror.tags[1] == addrTag;
        case 2:
            return
                mirror.tags[0] == addrTag ||
                mirror.tags[1] == addrTag;
        }
        return {};
    }
    void CtrlCop::loadCacheLine(u32 address, EeMipsCore& mips, CacheMode mode) {
        auto masterIndex{
            getCachePfn(address, mode)};
        auto writableCache{
            getCache(address, true, mode)};

        assignFlushedCache(writableCache, masterIndex);
        masterIndex |= dirtyBit;

        if ((writableCache.tags[0] != masterIndex &&
                writableCache.tags[1] != masterIndex)) {
            throw Cop0Err("No portion of the cache line {:#x} was properly selected! Tags: {:#x}",
                masterIndex, fmt::join(writableCache.tags, ", "));
        }
        // Due to the LRF algorithm, we will write to the way that was written last (thus keeping
        // the last data among the ways in the cache, waiting for one more miss)
        u8 primaryWay{writableCache.lrf[0] && !writableCache.lrf[1]};
        if (!primaryWay) {
            if (!writableCache.lrf[0] && writableCache.lrf[1])
                primaryWay = 2;
        }
        if (!primaryWay)
            // Means that the entire cache line is available for writing
            primaryWay = 255;
        u8 missPenalty{40};

        switch (primaryWay) {
        case 0xff:
            writableCache.ways[1].vec[1] = mips.mipsRead<os::vec>((address + 64));
            writableCache.ways[1].vec[1] = mips.mipsRead<os::vec>((address + 64) + 16);
            writableCache.ways[1].vec[2] = mips.mipsRead<os::vec>((address + 64) + 16 * 2);
            writableCache.ways[1].vec[3] = mips.mipsRead<os::vec>((address + 64) + 16 * 3);
            missPenalty *= 4;
            primaryWay = 1;
        case 1 ... 2:
            writableCache.ways[primaryWay - 1].vec[0] = mips.mipsRead<os::vec>(address + 0);
            writableCache.ways[primaryWay - 1].vec[1] = mips.mipsRead<os::vec>(address + 16);
            writableCache.ways[primaryWay - 1].vec[2] = mips.mipsRead<os::vec>(address + 16 * 2);
            writableCache.ways[primaryWay - 1].vec[3] = mips.mipsRead<os::vec>(address + 16 * 3);

            if (primaryWay != 0xff)
                missPenalty *= 2;
            break;
        }
        mips.runCycles -= missPenalty;
    }
    u32 CtrlCop::getCachePfn(u32 address, CacheMode mode) {
        if (mode == Instruction)
            return static_cast<u16>(address >> 13);
        else
            return static_cast<u16>(address >> 12);
    }

    void CtrlCop::assignFlushedCache(CopCacheLine& mixedCache, u32 tag, CacheMode mode) {
        // The EE uses a Least Recently Filled (LRF) algorithm to
        // determine which way to load data into
        u32 assign{};
        const std::array<u32, 2> mix{
            mixedCache.tags[0] & dirtyBit, mixedCache.tags[1] & dirtyBit
        };

        if (mix[0] && !mix[1])
            assign = 1;
        if (mix[1] && !mix[0])
            assign = 2;

        if (assign) {
            assign--;
            mixedCache.lrf[assign] = true;
        } else {
            // The row to fill is the XOR of the LFU bits
            assign = (mixedCache.lrf[0] ^ mixedCache.lrf[1]);
            mixedCache.lrf[assign] = true;
        }
        // Here is where we write the tag bits
        mixedCache.tags[assign] = tag | dirtyBit;
    }
    CopCacheLine& CtrlCop::getCache(u32 mem, bool write, CacheMode mode) {
        u32 cacheIndex;
        std::span<CopCacheLine> selectedCache;
        if (mode == Instruction) {
            cacheIndex = (mem >> 6) & 0x7f;
            selectedCache = inCache;
        } else {
            cacheIndex = (mem >> 6) & 0x3f;
            selectedCache = dataCache;
        }
        const auto firstWayLayer{selectedCache[cacheIndex].tags[0]};

        const auto secondWayLayer{selectedCache[cacheIndex].tags[1]};

        std::array<Optional<u8*>, 2> maps{
            Optional(virtMap[firstWayLayer >> 12]),
            Optional(virtMap[secondWayLayer >> 12])
        };
        const auto firstLrf{selectedCache[cacheIndex].lrf[0]};
        const auto secondLrf{selectedCache[cacheIndex].lrf[1]};

        for (u32 layers{}; layers < 2; layers++) {
            if (maps[0] == virtMap[mem >> 12] && layers == 0 ? firstLrf : secondLrf)
                return selectedCache[cacheIndex];
        }

        const u32 way{((firstWayLayer >> 6) & 1) ^ ((secondWayLayer >> 6) & 1)};

        const auto isDirty{static_cast<bool>(
                way == 0 ? firstWayLayer & dirtyBit : secondWayLayer & dirtyBit)};

        if (write && mode == Data && isDirty) {
            uintptr_t wrm{*(*maps[way]) + (mem & 0xfc0)};
            BitCast<u64*>(wrm)[0] = selectedCache[cacheIndex].ways[way].large[0];
            BitCast<u64*>(wrm)[1] = selectedCache[cacheIndex].ways[way].large[1];
            BitCast<u64*>(wrm)[2] = selectedCache[cacheIndex].ways[way].large[2];
            BitCast<u64*>(wrm)[3] = selectedCache[cacheIndex].ways[way].large[3];
            BitCast<u64*>(wrm)[4] = selectedCache[cacheIndex].ways[way].large[4];
            BitCast<u64*>(wrm)[5] = selectedCache[cacheIndex].ways[way].large[5];
            BitCast<u64*>(wrm)[6] = selectedCache[cacheIndex].ways[way].large[6];
            BitCast<u64*>(wrm)[7] = selectedCache[cacheIndex].ways[way].large[7];
        }
        if (write) {
            // If we are writing to the cache, the dirty bit must be set
            selectedCache[cacheIndex].tags[way] |= dirtyBit;
        }
        return selectedCache[cacheIndex];
    }
}
