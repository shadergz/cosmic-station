#pragma once

#include <span>

#include <os/neon_simd.h>
#include <mio/mmu_tlb.h>
#include <mio/dma_parallel.h>

#include <eeiv/ee_fuji.h>
#include <eeiv/ee_flow_ctrl.h>

#include <eeiv/c0/cop0.h>
#include <eeiv/fu/cop1_fu.h>
#include <eeiv/timer/ee_timers.h>
namespace zenith::eeiv {
    enum class EEExecutionMode : u8 {
        // JIT compiler, the fastest option but with various interpretation issues
        JitRe,
        // Increases instruction decoding speed through cached blocks, which is faster
        // than a simple interpreter
        CachedInterpreter
    };

    class EEMipsCore : public EEFlowCtrl {
        static constexpr u8 countOfGPRs{32};
    public:
        EEMipsCore(std::shared_ptr<mio::DMAController>& dma);
        ~EEMipsCore();

        void resetCore();
        void pulse(u32 cycles);
        u32 fetchByPC();

        u32 writeArray(u32 address, std::span<u32> dataBlk);
        template<typename T>
        void directWrite(u32 address, T value) {
            auto pageNumber{address / 4096};
            auto page{tlbMap[pageNumber]};
            auto firstPage{reinterpret_cast<u8*>(1)};

            [[likely]] if (page > firstPage) {
                eeTLB->tlbChModified(pageNumber, true);
                *reinterpret_cast<T*>(memory->makeRealAddress(address & 4095)) = value;
            }
        }
        template <typename T>
        T tableRead(u32 address) {
            auto virtMem0{tlbMap[address / 4096]};
            return *reinterpret_cast<T*>(&virtMem0[address & 4095]);
        }
        template <typename T>
        inline auto gprAt(u32 index) {
            return reinterpret_cast<T>(GPRs[index].words[0]);
        }
        inline void chPC(u32 newPC) {
            lastPC = eePC;
            eePC = newPC;
        }

        void branchByCondition(bool cond, i32 jumpRel);
        void branchOnLikely(bool cond, i32 jumpRel);

        mio::TLBPageEntry* fetchTLBFromCop(u32* c0Regs);
        void updateTlb();
        void setTLBByIndex();

        void handleException(u8 el, u32 exceptVec, u8 code);

        bool isABranch{};
        u32 delaySlot{};

        EEExecutionMode procCpuMode{EEExecutionMode::CachedInterpreter};
        c0::CoProcessor0 cop0;
        fu::CoProcessor1 fuCop1;

        EEPC eePC{}, lastPC{};
        timer::EETimers timer;

        union eeRegister {
            eeRegister() {}
            struct {
                os::machVec128 qw{0, 0};
                union {
                    std::array<i64, 2> sdw;
                    std::array<u64, 2> dw;
                };
                union {
                    std::array<u32, 4> words;
                    std::array<i32, 4> swords;
                };
                std::array<u16, 8> hw;
            };
            u8 bytes[16];
        };
        eeRegister* GPRs;
        u32 sa;

    private:
        std::shared_ptr<mio::GlobalMemory> memory;
        std::shared_ptr<mio::TLBCache> eeTLB;
        // Current virtual table being used by the processor
        u8** tlbMap{};

        // Class that provides CPU code execution functionality
        std::unique_ptr<EEExecutor> eeExecutor;
        u8 irqTrigger{};
    };
}
