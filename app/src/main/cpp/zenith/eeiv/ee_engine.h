#pragma once

#include <eeiv/cop0.h>
#include <os/expanded_types.h>

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

    class EmotionMIPS {
        static constexpr uint countOfGPRs{32};
    public:
        EmotionMIPS();
        ~EmotionMIPS();

        void resetCore();
    private:
        EEExecutionMode execModel{EEExecutionMode::Interpreter};
        uint8_t* mainRamBlock;

        union eeRegister {
            eeRegister() : dw{0, 0} {}
            os::native128 qw;
            uint64_t dw[2];
            uint32_t words[4];
            uint16_t hw[8];
            uint8_t rwBytes[16];
        };

        eeRegister* gprs;

        uint32_t regPC{};
        CoProcessor0 coCPU0{};
    };

}

