#include <map>
#include <common/global.h>
#include <engine/copfpu/cop1_fu.h>
#define DSP_UO_VALUES 1
namespace cosmic::engine::copfpu {
    const std::string CoProcessor1::fpuGpr2String(u8 id) const {
        std::array<char, 8> fun{};
        if (id <= 31)
            std::snprintf(fun.data(), fun.size(), "f%2u", id);
        else
            std::snprintf(fun.data(), fun.size(), "fINV??");
        return {fun.data()};
    }

    CoProcessor1::CoProcessor1() {
    }

    void CoProcessor1::resetFlu() {
        f512 zero{};

        acc = {};
        status = {};

        fpuId.decimal = 0x2e59;
        auto furs{reinterpret_cast<f64*>(fprRegs.data())};

        vst1q_f64_x4(&furs[4 * 0], zero);
        vst1q_f64_x4(&furs[4 * 1], zero);
        vst1q_f64_x4(&furs[4 * 2], zero);
        vst1q_f64_x4(&furs[4 * 3], zero);
    }
    f32 CoProcessor1::sony754con(u32 value) {
        switch (value & 0x7f800000) {
        case 0:
            return bit_cast<f32>(value & 0x80000000);
        case 0x7f800000:
            return bit_cast<f32>((value & 0x80000000) | 0x7f7fffff);
        }
        return bit_cast<f32>(value);
    }
    u32 CoProcessor1::c1cfc(u8 index) {
        if (!index)
            return static_cast<u32>(fpuId.decimal);
        if (index == 0x1f) {
            return 0;
        }
        return {};
    }
    void CoProcessor1::checkOverflow(u8 reg) {
        bool isOverflowed{(fprRegs[reg].un & ~0x80000000) == 0x7f800000};
        if (isOverflowed) {
            fprRegs[reg].decimal = sony754con(fprRegs[reg].un);
            status.overflow = true;
#if DSP_UO_VALUES
            userLog->info("(Cop1): Register {} with value {:f} has overflowed", fpuGpr2String(reg), fprRegs[reg].decimal);
#endif
        } else {
            status.overflow = false;
        }
    }
    void CoProcessor1::checkUnderflow(u8 reg) {
        bool isUnder{(fprRegs[reg].un & 0x7f800000) == 0 &&
            (fprRegs[reg].un & 0x7FFFFF) != 0};
        if (isUnder) {
            std::array<f32, 2> deci{};

            deci[0] = fprRegs[reg].decimal;
            deci[1] = sony754con(fprRegs[reg].un);
            fprRegs[reg].decimal = deci[1];
            status.underflow = true;
#if DSP_UO_VALUES
            userLog->info("The {} register has underflow-ed, from {} to {}", fpuGpr2String(reg), deci[0], deci[1]);
#endif
        } else {
            status.underflow = false;
        }
    }
}