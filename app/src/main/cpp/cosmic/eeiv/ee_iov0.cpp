#include <eeiv/ee_engine.h>
namespace cosmic::eeiv {
    u32 EEMipsCore::writeArray(u32 address, std::span<u32> dataBlk) {
        u32 count{};
        for (const auto value : dataBlk) {
            directWrite<u32>(address++, value);
            count++;
        }
        return count;
    }
}
