#include <eeiv/ee_engine.h>

namespace zenith::eeiv {
    u32 EEMipsCore::writeArray(u32 address, std::span<u32> dataBlk) {
        u32 count{};
        std::for_each(dataBlk.begin(), dataBlk.end(), [this, &count, &address](const auto value){
            directWrite<u32>(address++, value);
            count++;
        });
        return count;
    }
}
