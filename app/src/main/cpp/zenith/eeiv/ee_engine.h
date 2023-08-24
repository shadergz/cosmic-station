#pragma once

#include <eeiv/cop0.h>
#include <os/neon_types.h>

#include <console/global_memory.h>

namespace eeiv {
    enum EEExecutionMode {
        // Translate opcode by opcode in an instruction table, accuracy is guaranteed
        Interpreter,
        // Increases instruction decoding speed through cache blocks, which is faster
        // than a simple interpreter
        CachedInterpreter,
        // JIT compiler, the fastest option but with various interpretation issues
        JitRe
    };

    class EEMipsCore {
        static constexpr uint countOfGPRs{32};
    public:
        EEMipsCore(const std::shared_ptr<console::GlobalMemory>& memoryChips);
        ~EEMipsCore();

        void resetCore();
    private:
        EEExecutionMode m_execModel{EEExecutionMode::Interpreter};

        std::shared_ptr<console::GlobalMemory> m_glbMemory;
        os::MappedMemory<uint8_t> m_mainRamBlock;

        union eeRegister {
            eeRegister()
                : dw{0, 0} {}
            os::native128 qw;
            uint64_t dw[2];
            uint32_t words[4];
            uint16_t hw[8];
            uint8_t rwBytes[16];
        };
        eeRegister* m_gprs;

        uint32_t m_eePC{};
        ext::CoProcessor0 m_copCPU0{};
    };
}
