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
    void EEMipsCore::setLoHi(i64 lo, i64 hi) {
        mulDivStorage[0] = lo & 0xffffffff;
        mulDivStorage[1] = hi & 0xffffffff;
    }
    void EEMipsCore::setLoHi(u64 split) {
        i64 val{bit_cast<i64>(split)};
        mulDivStorage[0] = val & 0xffffffff;
        mulDivStorage[1] = (val >> 32) & 0xffffffff;
    }
}
