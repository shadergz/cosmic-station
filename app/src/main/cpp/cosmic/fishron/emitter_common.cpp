#include <fishron/emitter_common.h>

namespace cosmic::fishron {
    enum Arm64Instruction {
        Str, Stp
    };
    struct Arm64Scaled {
        Arm64Instruction arm;
        u32 mask;
    };

    std::array<Arm64Scaled, 2> lookScaled{
        Arm64Scaled{Str, 0xb8000fe0}, // str w0, [sp, #-4]!
        Arm64Scaled{Stp, 0x29000000}  // stp w0, w1, [sp, #-8]!
    };
    std::array<std::string, 2> emitterFormats {
        "str w%u, [sp, #03u]!",
        "stp w%u, w%u, [sp, #3u]!"
    };
    JitBlockRegisters::JitBlockRegisters() {
        for (u8 maid{}; maid < 31; maid++) {
            intRegisters[maid].registerSet = IntRegister << 8 | maid;
            longRegisters[maid].registerSet = LongRegister << 8 | maid;

            singleRegisters[maid].registerSet = SingleRegister << 8 | maid;
            doubleRegisters[maid].registerSet = DoubleRegister << 8 | maid;
            neonRegisters[maid].registerSet = NeonRegister << 8 | maid;
        }
    }
    std::array<u16, 4> JitBlockRegisters::allocate(ArmV8aRegister set, u32 regsCount) {
        if (!regsCount)
            return {};
        u8 refer{}, referMax{};
        std::array<u16, 4> reSet{};
        u8 recursive{};
        std::span<Arm64v8Gpr> gprs;

        switch (set) {
        case IntRegister:
            refer = w8;
            referMax = w18;
            gprs = intRegisters;
            break;
        case LongRegister:
        case SingleRegister:
        case DoubleRegister:
        case NeonRegister:
            break;
        }

        for (; refer != referMax; refer++) {
            for (u8 in{static_cast<u8>(refer + 1)}; in != referMax; in++) {
                if (gprs[refer] < gprs[in]) {
                    reSet[recursive++] = gprs[refer].registerSet;
                }
                if (recursive >= reSet.size())
                    recursive = 0;
            }
        }
        return reSet;
    }

    void JitBlockRegisters::deallocate(std::span<u16> regs) {
    }
    void Emitter::stackPushGpr(JitBlock& block, std::span<Arm64v8Gpr> regs) {
        u8 allocated{};
        while (regs.size() != allocated) {
            if (regs.size() - allocated >= 2) {
                block.text.write(lookScaled[Stp].mask); // STP
                allocated += 2;
            } else {
                block.text.write(lookScaled[Str].mask); // STR
                allocated++;
            }
        }
    }
    void Emitter::stackPopGpr(JitBlock& block, std::span<Arm64v8Gpr> regs) {
    }

    u8 JitBlock::saveRegisters(std::span<u16> sets) {
        if (sets.size() > 4 || sets.empty())
            return 0;
        std::array<Arm64v8Gpr, 4> gprs;
        std::span<Arm64v8Gpr> regs;

        bool itsFine{sets[0] >> 8 == sets[1] >> 8 == sets[2] >> 8 == sets[3] >> 8};
        if (sets[0] >> 8 == IntRegister)
            regs = regFiles.intRegisters;

        if (itsFine) {
            for (u8 lp{}; lp < 4; lp++) {
                regs[lp].referCount++;
                gprs[lp] = regs[lp];
            }
            jitter->stackPushGpr(*this, gprs);
        }
        return itsFine ? gprs.size() : 0;
    }

    void JitBlock::restoreRegisters(std::span<u16> sets) {
    }
}
