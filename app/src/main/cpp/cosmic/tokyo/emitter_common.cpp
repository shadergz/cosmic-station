#include <tokyo/emitter_common.h>

namespace cosmic::tokyo {
    Arm64Gpr::Arm64Gpr() {
        ender->pearl->stackPushGpr(*this);
    }

    Arm64Gpr::~Arm64Gpr() {
    }

    Arm64Vec::Arm64Vec() {
    }

    Arm64Vec::~Arm64Vec() {
    }

    std::array<u32, 1> emitterArm {
        0xb8000fe0 // str w0, [sp, #0]!
    };
    std::array<const char*, 1> emitterFormats {
        "str w%u, [sp, #03u]!"
    };

    StackOperation Emitter::stackPushGpr(Arm64Gpr& gpr) {
        auto v8a{lookasideRegister(gpr)};
        StackOperation stack{.spo = -4};

        switch (v8a.reg) {
        case IntRegister:
            // str w?, [sp, #-4]!
            stack.operate = IntRegister << 8 | 0;
            stack.arm = emitterArm[0];
            break;
        }
        return stack;
    }
    StackOperation Emitter::stackPopGpr(Arm64Gpr& gpr) {
        return {};
    }

    StackOperation Emitter::stackPushVec(Arm64Vec& vec) {
        return {};
    }
    StackOperation Emitter::stackPopVec(Arm64Vec& vec) {
        return {};
    }

    Emitter::RangedRegister Emitter::lookasideRegister(Arm64Gpr& gpr) {
        auto intRanges{gpr.ender->regFiles.intRegisters.data()};
        auto endIntRanges{intRanges + gpr.ender->regFiles.intRegisters.size()};

        if (&gpr >= intRanges || &gpr <= endIntRanges) {
            return {.begin = *intRanges, .end = *endIntRanges, .reg = IntRegister};
        }
        return {};
    }
    Emitter::RangedRegister Emitter::lookasideRegister(Arm64Vec& vec) {
        return {};
    }

    JitBlockRegisters::JitBlockRegisters() {
        intRegisters.resize(31);
        longRegisters.resize(31);

        singleRegisters.resize(31);
        doubleRegisters.resize(31);
        neonRegisters.resize(31);
    }
}
