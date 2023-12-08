#pragma once
#include <vector>
#include <variant>
#include <tokyo/jitter_arm64.h>
namespace cosmic::tokyo {
    enum ArmV8aRegister {
        IntRegister,
        LongRegister,
        SingleRegister,
        DoubleRegister,
        NeonRegister
    };
    enum ArmV8aAbiGprs {
        x0 = 0,
        w0 = 0
    };
    enum ArmV8aAbiVec {

    };
    class JitBlock;
    class Arm64Gpr {
    public:
        Arm64Gpr();
        ~Arm64Gpr();

        raw_reference<JitBlock> ender;
    };
    class Arm64Vec {
    public:
        Arm64Vec();
        ~Arm64Vec();

        raw_reference<JitBlock> ender;
    };
    class JitBlockRegisters {
    public:
        JitBlockRegisters();
        std::vector<Arm64Gpr> intRegisters;
        std::vector<Arm64Gpr> longRegisters;

        std::vector<Arm64Vec> singleRegisters;
        std::vector<Arm64Vec> doubleRegisters;
        std::vector<Arm64Vec> neonRegisters;
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

    class JitBlock {
    public:
        JitBlock() : text(executable) {}
        raw_reference<Emitter> pearl{};
        JitBlockRegisters regFiles{};
        u32 executable[2];
        TextSegment text;
    };

    class Emitter {
    public:
        StackOperation stackPushGpr(Arm64Gpr& gpr);
        StackOperation stackPopGpr(Arm64Gpr& gpr);

        StackOperation stackPushVec(Arm64Vec& vec);
        StackOperation stackPopVec(Arm64Vec& vec);

        using a64Gpr = raw_reference<Arm64Gpr>;
        using a64Vec = raw_reference<Arm64Vec>;

        struct RangedRegister {
            std::variant<a64Gpr, a64Vec> begin{};
            std::variant<a64Gpr, a64Vec> end{};
            u8 reg{};
        };

        RangedRegister lookasideRegister(Arm64Gpr& vec);
        RangedRegister lookasideRegister(Arm64Vec& vec);
    };
}