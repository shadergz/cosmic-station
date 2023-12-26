#include <engine/copfpu/cop1_fu.h>
namespace cosmic::engine::copfpu {
    CoProcessor1::CoProcessor1() {
    }

    void CoProcessor1::resetFlu() {
        auto fuRegs{bit_cast<f64*>(fprRegs.data())};
        f512 zero{};

        acc = {};
        status = {};

        fpuId.decimal = 0x2e59;
        vst1q_f64_x4(fuRegs + 0, zero);
        vst1q_f64_x4(fuRegs + 2 * 4, zero);
        vst1q_f64_x4(fuRegs + 2 * 4 * 2, zero);
        vst1q_f64_x4(fuRegs + 2 * 4 * 3, zero);
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
        } else {
            status.overflow = false;
        }
    }
    void CoProcessor1::checkUnderflow(u8 reg) {
        bool isUnder{(fprRegs[reg].un & 0x7f800000) == 0 &&
            (fprRegs[reg].un & 0x7FFFFF) != 0};
        if (isUnder) {
            fprRegs[reg].decimal = sony754con(fprRegs[reg].un);
            status.underflow = true;
        } else {
            status.underflow = false;
        }
    }
}