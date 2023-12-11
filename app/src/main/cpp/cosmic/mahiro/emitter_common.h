#pragma once
#include <array>
#include <vector>
#include <common/types.h>
namespace cosmic::mahiro {
    enum ArmV8aRegister : u8 {
        IntRegister,
        LongRegister,
        SingleRegister,
        DoubleRegister,
        NeonRegister
    };
    enum ArmV8aAbiGprs : u8 {
        w0 = 0,
        w8 = 8,
        w18 = 18,
    };
    enum ArmV8aAbiVec : u8 {};
    struct Arm64v8Gpr {
        u16 registerSet;
        u32 referCount;

        auto operator <(Arm64v8Gpr& gpr) {
            return referCount < gpr.referCount;
        }
    };

    class JitBlockRegisters {
    public:
        JitBlockRegisters();
        std::array<Arm64v8Gpr, 31> intRegisters;
        std::array<Arm64v8Gpr, 31> longRegisters;

        std::array<Arm64v8Gpr, 31> singleRegisters;
        std::array<Arm64v8Gpr, 31> doubleRegisters;
        std::array<Arm64v8Gpr, 31> neonRegisters;

        std::array<u16, 4> allocate(ArmV8aRegister set, u32 regsCount);
        void deallocate(std::span<u16> regs);
    };
    class Emitter;
    class TextSegment {
    public:
        TextSegment(std::span<u32> segment) : exe(segment) {
            pos = 0;
        }
        void write(u32 instruction) {
            exe[pos++] = instruction;
        }
        u64 pos;
        std::span<u32> exe;
    };

    class Emitter;
    class JitBlock {
    public:
        JitBlock() : text(executable) {}
        JitBlockRegisters regFiles{};
        u32 executable[2];
        TextSegment text;
        u32 pc;
        u32 page;

        u8 saveRegisters(std::span<u16> sets);
        void restoreRegisters(std::span<u16> sets);

        std::shared_ptr<Emitter> jitter;
    };

    class Emitter {
    public:
        void stackPushGpr(JitBlock& block, std::span<Arm64v8Gpr> regs);
        void stackPopGpr(JitBlock& block, std::span<Arm64v8Gpr> regs);

        std::vector<JitBlock> blocks;
    };
}