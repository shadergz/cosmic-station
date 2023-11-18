#include <cpu/verify_features.h>

namespace cosmic::cpu {
    const std::array<FeaturesImpl, ISA64::size> features{
        {{true, "crc32cb,crc32ch,crc32cw,crc32cx"}}
    };

    HostFeatures::HostFeatures() {
        u64 el1;
        asm volatile("mrs %0, ID_AA64ISAR0_EL1" : "=r" (el1));

        for (u32 feat{}; feat < features.size(); feat++) {
            u8 have{};
            switch (feat) {
            case ISA64::Crc32:
                have = ((el1 >> 4) & 0xf) != 0;
                break;
            }
            if (!features[feat].isArchOptional && !have) {
                throw AppFail("Your CPU SoC doesn't support the required family of instructions {}", features[feat].family);
            }
            mrsA64.set(feat, have);
        }
    }

    bool HostFeatures::haveCrc32C() {
        return mrsA64[ISA64::Crc32];
    }
}
