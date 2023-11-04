#pragma once

#include <span>

#include <os/neon_simd.h>
#include <link/blocks.h>
#include <eeiv/mmu_tlb.h>
#include <eeiv/ee_handler.h>
#include <eeiv/ee_flow_ctrl.h>

#include <eeiv/c0/cop0.h>
#include <eeiv/fu/cop1_fu.h>
#include <eeiv/timer/ee_timers.h>
namespace zenith::eeiv {
    enum class EEExecutionMode : u8 {
        // JIT compiler, the fastest option but with various interpretation issues
        JitRe,
        // Increases instruction decoding speed through cache blocks, which is faster
        // than a simple interpreter
        CachedInterpreter
    };

    class EEMipsCore : public EEFlowCtrl {
        static constexpr u8 countOfGPRs{32};
    public:
        EEMipsCore(std::shared_ptr<link::GlobalMemory>& global);
        ~EEMipsCore();

        void resetCore();
        void pulse(u32 cycles);
        u32 fetchByPC();

        u32 writeArray(u32 address, std::span<u32> dataBlk);
        template<typename T>
        void directWrite(u32 address, T value) {
            auto pageNumber{address / 4096};
            auto page{virtTable[pageNumber]};
            auto firstPage{reinterpret_cast<u8*>(1)};

            [[likely]] if (page > firstPage) {
                eeTLB->tlbChModified(pageNumber, true);
                *reinterpret_cast<T*>(memory->makeRealAddress(address & 4095)) = value;
            }
        }
        template <typename T>
        T tableRead(u32 address) {
            auto virtMem0{virtTable[address & 4095]};
            return *reinterpret_cast<T*>(virtMem0);
        }

        EEExecutionMode proCPUMode{EEExecutionMode::CachedInterpreter};
        c0::CoProcessor0 cop0;
        fu::CoProcessor1 fuCop1;

        EEPC eePC{}, lastPC{};
        timer::EETimers timer;
    private:

        std::shared_ptr<link::GlobalMemory> memory;
        union eeRegister {
            eeRegister() {}
            struct {
                os::machVec128 qw{0, 0};
                u64 dw[2];
                u32 words[4];
                u16 hw[8];
            };
            u8 rawBytes[16];
        };

        eeRegister* GPRs;
        std::shared_ptr<TLBCache> eeTLB;
        // Current virtual table being used by the processor
        u8** virtTable{};

        // Class that provides CPU code execution functionality
        std::unique_ptr<EEExecutor> eeExecutor;
        u8 irqTrigger{};
    };
}
