#include <engine/ee_core.h>
namespace cosmic::engine {
    u32 EeMipsCore::writeArr(u32 address, std::span<u32> dataBlk) {
        u32 count{};
        for (const auto value : dataBlk) {
            mipsWrite<u32>(address++, value);
            count++;
        }
        return count;
    }
    void EeMipsCore::setLoHi(i64 lo, i64 hi) {
        mulDivStorage[0] = lo & 0xffffffff;
        mulDivStorage[1] = hi & 0xffffffff;
    }
    void EeMipsCore::setLoHi(u64 split) {
        i64 val{BitCast<i64>(split)};
        mulDivStorage[0] = val & 0xffffffff;
        mulDivStorage[1] = (val >> 32) & 0xffffffff;
    }
    const std::array<const char*, 32> eeAllGprIdentifier{
        "zero",
        "at", "v0", "v1", "a0", "a1", "a2", "a3",
        "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
        "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
        "t8", "t9",
        "k0", "k1",
        "gp", "sp", "fp", "ra"
    };
}
