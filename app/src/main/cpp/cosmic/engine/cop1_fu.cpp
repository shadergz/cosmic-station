#include <map>
#include <common/global.h>
#include <engine/cop1_fu.h>

#include <fmt/format.h>
#define DSP_UO_VALUES 1
namespace cosmic::engine {
    const std::string FpuCop::fpuGpr2String(u8 id) const {
        std::string regName{"fINV??"};
        if (id <= 31) {
            regName = fmt::format("f{:02}", id);
        }
        return regName;
    }

    FpuCop::FpuCop() {
    }

    void FpuCop::resetFlu() {
        f512 zero;
        f128 zero8;

        std::memset(&zero, 0x0, sizeof(f512));
        std::memset(&zero8, 0x0, sizeof(zero8));

        f64* array;
        acc = {};
        status = {};

        fpuId.decimal = 0x2e59;
        array = BitCast<f64*>(fprRegs.data());

        vst1q_f64_x4(&array[4 * 0], zero);
        for (u8 pe{}; pe < 4; pe++)
            *BitCast<f128*>(&array[(4 * 2) + (2 * pe)]) = zero8;
    }
    f32 FpuCop::sony754con(u32 value) {
        switch (value & 0x7f800000) {
        case 0:
            return static_cast<f32>(value & 0x80000000);
        case 0x7f800000:
            return static_cast<f32>((value & 0x80000000) | 0x7f7fffff);
        }
        return static_cast<f32>(value);
    }
    u32 FpuCop::c1cfc(u8 index) {
        if (!index)
            return static_cast<u32>(fpuId.decimal);
        if (index == 0x1f) {
            return 0;
        }
        return {};
    }
    void FpuCop::checkOverflow(u8 reg) {
        u32 value;
        if (reg == 32)
            value = acc.un;
        else
            value = fprRegs[reg].un;

        bool isOverflowed{(value & ~0x80000000) == 0x7f800000};
        if (isOverflowed) {
            if (reg != 32)
                fprRegs[reg].decimal = sony754con(value);
            else
                acc.decimal = sony754con(value);
            status.overflow = true;
#if DSP_UO_VALUES
            user->info("(COP1): Register {} with value {:f} has overflowed", fpuGpr2String(reg), fprRegs[reg].decimal);
#endif
        } else {
            status.overflow = false;
        }
    }
    void FpuCop::checkUnderflow(u8 reg) {
        FpuReg under{acc};
        if (reg < 32)
            under = fprRegs[reg];

        bool isUnder{(under.un & 0x7f800000) == 0 && (under.un & 0x7FFFFF) != 0};
        if (isUnder) {
            std::array<f32, 2> deci{};

            deci[0] = under.decimal;
            deci[1] = sony754con(under.un);
            under.decimal = deci[1];
            status.underflow = true;
#if DSP_UO_VALUES
            user->info("(COP1): The {} register has underflow-ed from {} to {}", fpuGpr2String(reg), deci[0], deci[1]);
#endif
        } else {
            status.underflow = false;
        }
    }
}