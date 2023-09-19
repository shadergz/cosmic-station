#pragma once

#include <os/neon_types.h>
#include <link/global_memory.h>
#include <eeiv/cop0.h>
#include <eeiv/high_cache.h>
#include <eeiv/mmu_tlb.h>

#include <eeiv/ee_handler.h>
namespace zenith::eeiv {
    enum class EEExecutionMode : u8 {
        // Increases instruction decoding speed through cache blocks, which is faster
        // than a simple interpreter
        CachedInterpreter,
        // JIT compiler, the fastest option but with various interpretation issues
        JitRe
    };

    class EEMipsCore {
        static constexpr u8 countOfGPRs{32};
        static constexpr u8 countOfCacheLines{128};
    public:
        EEMipsCore(const std::shared_ptr<console::GlobalMemory>& glbRef);
        ~EEMipsCore();

        void resetCore();

        EEExecutionMode m_eeExecMode{EEExecutionMode::CachedInterpreter};
    private:

        std::shared_ptr<console::GlobalMemory> m_glbRAM;
        union eeRegister {
            eeRegister()
                : dw{0, 0}
                {}
            struct {
                os::machVec128 qw{0, 0};
                u64 dw[2];
                u32 words[4];
                u16 hw[8];
            };
            u8 m_rwBytes[16];
        };

        eeRegister* m_GPRs;
        EECacheLine* m_eeNearCache;

        u32 m_eePC{};
        CoProcessor0 m_copCPU0;

        std::unique_ptr<TLBCache> m_eeTLB;
        // Class that provides CPU code execution functionality
        std::unique_ptr<EEExecutor> m_eeExecutor;

    };
}
