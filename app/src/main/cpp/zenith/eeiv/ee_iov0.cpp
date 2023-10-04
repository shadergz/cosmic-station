#include <functional>

#include <eeiv/ee_engine.h>

namespace zenith::eeiv {
    void EEMipsCore::write32(u32 address, u32 value) {
        auto pageNumber{address / 4096};
        auto page{virtTable[pageNumber]};
        auto firstPage{reinterpret_cast<u8*>(1)};

        [[likely]] if (page > firstPage) {
            eeTLB->tlbChModified(pageNumber, true);
            *reinterpret_cast<u32*>(&glbRDRAM->RDRAMBlk[address & 4095]) = value;
        }
    }

    u32 EEMipsCore::writeArray(u32 address, std::span<u32> dataBlk) {
        u32 count{};
        std::for_each(dataBlk.begin(), dataBlk.end(), [this, &count, &address](const auto value){
            write32(address++, value);
            count++;
        });
        return count;
    }
}
