#pragma once

#include <common/types.h>
namespace cosmic::ee {
    union FpuReg {
        f32 decimal;
        u32 un;
        i32 si;
    };
    struct Cop1Control {
        bool overflow;
        bool underflow;
    };

    class FpuCop {
    public:
        FpuCop();
        void resetFlu();
        f32 sony754con(u32 value);
        u32 c1cfc(u8 index);

        void checkOverflow(u8 reg);
        void checkUnderflow(u8 reg);

        union {
            std::array<FpuReg, 32> fprRegs;
        };
        FpuReg acc;
        FpuReg fpuId;
        Cop1Control status;
    private:
        const std::string fpuGpr2String(u8 id) const;
    };
}
