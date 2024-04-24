#include <arm_acle.h>

#include <cpu/cyclic32.h>
namespace cosmic::cpu {
    u32 check32(std::span<u8> chkData) {
        u32 crc{0xffffffff};
        u64 last{chkData.size()};

        u32* chucks{BitCast<u32*>(chkData.data())};

        for (; last >= sizeof(u32) * 8; ) {
            crc = __crc32cd(crc, *chucks++);
            crc = __crc32cd(crc, *chucks++);

            crc = __crc32cd(crc, *chucks++);
            crc = __crc32cd(crc, *chucks++);

            crc = __crc32cd(crc, *chucks++);
            crc = __crc32cd(crc, *chucks++);

            crc = __crc32cd(crc, *chucks++);
            crc = __crc32cd(crc, *chucks++);

            last -= sizeof(u32) * 8;
        }

        for (; last >= sizeof(u32); ) {
            crc = __crc32cd(crc, *chucks++);
            last -= sizeof(u32);
        }

        u8* pieces{BitCast<u8*>(chucks)};

        for (; last;  ) {
            crc = __crc32b(crc, *pieces++);
            last -= sizeof(u8);
        }
        return ~crc;
    }
}

