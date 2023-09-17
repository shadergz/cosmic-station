#pragma once

#include <functional>
#include <impl_types.h>

namespace zenith::eeiv::cop {
    static constexpr u8 cop0RegsCount{32};

    union Cop0Status {
        u32 rawStatus{};
        struct {
            u32 interruptEnable: 1;
            u32 exceptionLevel: 1;
            u32 errorLevel: 1;
            // kernelPr == 0 -> Kernel mode
            // kernelPr == 1 -> Supervisor mode
            // kernelPr == 2 -> User mode
            u32 kernelPr: 2;
            u32: 5;
            u32 interruption0: 1;
            u32 interruption1: 1;
            u32 busCouldFail: 1;
            u32: 2;
            u32 interruption5: 1;
            u32 interrupt99999: 1;
            u32 edi: 1;
            u32 cacheHit: 1;
            u32: 3;
            u32 bev: 1;
            u32 dev: 1;
            u32 unknownYet: 4;
            u32 copUsable: 4;
        };
    };

    class CoProcessor0 {
    public:
        CoProcessor0();

        CoProcessor0(CoProcessor0&& copMove) = delete;
        CoProcessor0(CoProcessor0& copCopy) = delete;

#pragma pack(push, 4)
        struct CoStatus {
            // The arrays of hwReservedX are all the registers reserved by the hardware manufacturer
            u32 index;
            u32 random;
            u32 entryLo0;
            u32 entryLo1;
            u32 context;
            u32 pageMask;
            u32 wired;
            u32 hwReserved0[1];
            u32 badVAddress;
            u32 count;
            u32 entryHi;
            u32 compare;
            Cop0Status status;
            u32 cause;
            u32 epc;

            // The codenamed pRid register determines in the very early boot process for the BIOS
            // which processor it is currently running on, whether it's on the EE or the PSX
            u32 pRid;

            u32 config;
            u32 hwReserved1[6];
            u32 badPAddress;
            u32 debug;
            u32 perf;
            u32 hwReserved2[2];
            u32 tagLo;
            u32 tagHi;
            u32 errorEPC;
        } cops;
        static_assert(__builtin_offsetof(CoStatus, pRid) == sizeof(u32) * 15);
#pragma pack(pop)

    private:
        u32 m_copGPRs[cop0RegsCount]{};
    };
    static_assert(sizeof(u32) * cop0RegsCount == 128);
}

