#pragma once

#include <functional>
#include <impl_types.h>

namespace zenith::eeiv::cop {
    static constexpr uint cop0RegCount1{31};

    union Cop0Status {
        uint32_t rawStatus{};
        struct {
            uint32_t interruptEnable: 1;
            uint32_t exceptionLevel: 1;
            uint32_t errorLevel: 1;
            // kernelPr == 0 -> Kernel mode
            // kernelPr == 1 -> Supervisor mode
            // kernelPr == 2 -> User mode
            uint32_t kernelPr: 2;
            uint32_t: 5;
            uint32_t interruption0: 1;
            uint32_t interruption1: 1;
            uint32_t busCouldFail: 1;
            uint32_t: 2;
            uint32_t interruption5: 1;
            uint32_t interrupt99999: 1;
            uint32_t edi: 1;
            uint32_t cacheHit: 1;
            uint32_t: 3;
            uint32_t bev: 1;
            uint32_t dev: 1;
            [[maybe_unused]] uint32_t unknownYet: 4;
            uint32_t copUsable: 4;
        };
    };

    union CoProcessor0 {
    public:
        CoProcessor0() {
            // Enabling level 2 exception handling and forcing level 1 exception handling, then
            // jumping to the bootstrap vectors
            // bev == 1 && errorLevel == 1
            status.rawStatus = 0x400004;
        }

        CoProcessor0(CoProcessor0&& copMove) = delete;
        CoProcessor0(CoProcessor0& copCopy) = delete;

#pragma pack(push, 4)
        struct {
            // The arrays of hwReservedX are all the registers reserved by the hardware manufacturer
            uint32_t index;
            uint32_t random;
            uint32_t entryLo0;
            uint32_t entryLo1;
            uint32_t context;
            uint32_t pageMask;
            uint32_t wired;
            [[maybe_unused]] uint32_t hwReserved0[1];
            uint32_t badVAddress;
            uint32_t count;
            uint32_t entryHi;
            uint32_t compare;
            Cop0Status status;
            uint32_t cause;
            uint32_t epc;

            // The codenamed pRid register determines in the very early boot process for the BIOS
            // which processor it is currently running on, whether it's on the EE or the PSX
            uint32_t pRid;

            uint32_t config;
            [[maybe_unused]] uint32_t hwReserved1[6];
            uint32_t badPAddress;
            uint32_t debug;
            uint32_t perf;
            [[maybe_unused]] uint32_t hwReserved2[2];
            uint32_t tagLo;
            uint32_t tagHi;
            uint32_t errorEPC;
        };
#pragma pack(pop)

    private:
        [[maybe_unused]] uint32_t m_rawCopRegisters[cop0RegCount1] = {};
    };
    static_assert(sizeof(CoProcessor0) == sizeof(uint32_t) * cop0RegCount1);
    static_assert(__builtin_offsetof(CoProcessor0, pRid) == sizeof(uint32_t) * 15);
}

