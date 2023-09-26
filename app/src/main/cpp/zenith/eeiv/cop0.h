#pragma once

#include <functional>
#include <impl_types.h>
namespace zenith::eeiv {
    static constexpr u8 cop0RegsCount{32};

    union Cop0Status {
        u32 rawStatus{};
        struct {
            u8 copUsable;
        };
    };

    union CoProcessor0 {
    public:
        CoProcessor0();
        CoProcessor0(CoProcessor0&&) = delete;
        CoProcessor0(CoProcessor0&) = delete;

#pragma pack(push, 4)
        struct {
            // The codenamed pRid register determines in the very early boot process for the BIOS
            // which processor it is currently running on, whether it's on the EE or the PSX
            Cop0Status status;
            u32 pRid;
        };
#pragma pack(pop)
    private:
        u32 m_copGPRs[2];
    };

    static_assert(offsetof(CoProcessor0, pRid) == sizeof(u32) * 1);
    static_assert(sizeof(u32) * cop0RegsCount == 128);
}

