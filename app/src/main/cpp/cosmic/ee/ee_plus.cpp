#include <ee/ee_core.h>
namespace cosmic::ee {
    template <typename T>
    struct RangedAddress {
        u32 base;
        RangedAddress(u32 addr) : base(addr) {
        }
        u32 operator++(i32 inc) {
            auto before{base};
            base += sizeof(T);
            return before;
        }
        operator u32() const {
            return base;
        }
    };
    u64 EeMipsCore::writeArr(u32 address, std::span<u32> dataBlk) {
        u64 count{};
        RangedAddress<u32> addrW{address};

        for (; (dataBlk.size() - count) >= 8; count += 8) {
            mipsWrite<u32>(addrW++, dataBlk[count + 0]);
            mipsWrite<u32>(addrW++, dataBlk[count + 1]);
            mipsWrite<u32>(addrW++, dataBlk[count + 2]);
            mipsWrite<u32>(addrW++, dataBlk[count + 3]);
            mipsWrite<u32>(addrW++, dataBlk[count + 4]);
            mipsWrite<u32>(addrW++, dataBlk[count + 5]);
            mipsWrite<u32>(addrW++, dataBlk[count + 6]);
            mipsWrite<u32>(addrW++, dataBlk[count + 7]);

        }
        const std::span<u32> remain{
            &dataBlk[count],
            dataBlk.size() - count};
        for (const auto value : remain) {
            mipsWrite<u32>(addrW++, value);
        }
        return count + remain.size();
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
    const std::array<std::string, 32> eeAllGprIdentifier{
        "zero",
        "at", "v0", "v1", "a0", "a1", "a2", "a3",
        "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
        "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
        "t8", "t9",
        "k0", "k1",
        "gp", "sp", "fp", "ra"
    };
}
