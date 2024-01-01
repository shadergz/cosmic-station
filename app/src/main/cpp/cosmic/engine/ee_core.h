#pragma once

#include <span>

#include <os/neon_simd.h>
#include <mio/mmu_tlb.h>
#include <mio/mem_pipe.h>

#include <engine/ee_info.h>

#include <engine/copctrl/cop0.h>
#include <engine/cop1_fu.h>
#include <engine/ee_timers.h>
#include <vu/v01_cop2vu.h>
namespace cosmic::engine {
    class EePc {
    public:
        EePc() = default;
        EePc(u32 pc) : pcValue(pc) {}
        auto operator++(i32 inc) {
            return pcValue += 4;
        }
        auto operator--(i32 inc) {
            return pcValue -= 4;
        }
        auto operator*() {
            return pcValue;
        }
        u32 pcValue{};
    };
    struct EeFlowCtrl {
        i64 cycles[1],
            runCycles;
    };

    enum ExecutionMode : u8 {
        // JIT compiler, the fastest option but with various interpretation issues
        JitRe,
        // Increases instruction decoding speed through cached blocks, which is faster
        // than a simple interpreter
        CachedInterpreter
    };

    class EeMipsCore : public EeFlowCtrl {
        static constexpr u8 countOfGPRs{32};
    public:
        EeMipsCore(std::shared_ptr<mio::MemoryPipe>& pipe);
        ~EeMipsCore();

        void resetCore();
        void pulse(u32 cycles);
        u32 fetchByPc();
        u32 fetchByAddress(u32 address);
        void invalidateExecRegion(u32 address);

        u32 writeArr(u32 address, std::span<u32> dataBlk);
        const u8* first{reinterpret_cast<u8*>(1)};

        template <typename T>
        T mipsRead(u32 address) {
            const u32 virt{address / 4096};
            const u8* page{cop0.virtMap[virt]};
            bool br{page == first};
            if (br) {
                if constexpr (sizeof(T) == 4) {
                    return observer->readGlobal(address & 0x1fffffff, sizeof(T), mio::EngineDev).as<T>();
                }
            } else if (page > first) {
                return *reinterpret_cast<T*>(observer->controller->mapped->makeRealAddress(address & 0xfff, mio::MainMemory));
            }
            return {};
        }
        template<typename T>
        void mipsWrite(u32 address, T value) {
            const u32 pn{address / 4096};
            const u8* page{cop0.virtMap[pn]};
            [[unlikely]] if (page == first) {
                cop0.virtCache->tlbChangeModified(pn, true);
                observer->writeGlobal(address & 0x1fffffff, value, sizeof(value), mio::EngineDev);
            } else if (page > first) {
                auto target{reinterpret_cast<T*>(observer->controller->mapped->makeRealAddress(address & 0xfff))};
                *target = value;
            }
            invalidateExecRegion(address);
        }
        template <typename T>
        inline auto gprAt(u32 index) {
            return reinterpret_cast<T*>(&(GPRs[index].b[0]));
        }
        inline u32 incPc() {
            chPc(eePc++);
            return lastPc.pcValue;
        }
        inline void chPc(u32 neoPC) {
            lastPc = eePc;
            eePc = neoPC;
        }
        i64 getHtzCycles(bool total) const;
        void printStates();
        void branchByCondition(bool cond, i32 jumpRel);
        void branchOnLikely(bool cond, i32 jumpRel);

        RawReference<mio::TlbPageEntry> fetchTlbFromCop(u32* c0Regs);
        void updateTlb();
        void setTlbByIndex();

        void handleException(u8 el, u32 exceptVec, u8 code);

        void setLoHi(i64 lo, i64 hi);
        void setLoHi(u64 split);

        bool isABranch{};
        u32 delaySlot{};
        ExecutionMode procCpuMode{ExecutionMode::CachedInterpreter};
        copctrl::CtrlCop cop0;
        FpuCop cop1;
        std::unique_ptr<vu::MacroModeCop2> cop2;

        EePc eePc{},
            lastPc{};
        EeTimers timer;

        union eeRegister {
            eeRegister() {}
            os::vec qw{0};
            std::array<i64, 2> sdw;
            std::array<u64, 2> dw;
            std::array<u32, 4> words;
            std::array<i32, 4> swords;
            std::array<u16, 8> hw;
            u8 b[16];
        };
        std::array<eeRegister, countOfGPRs> GPRs;
        u32 sa;

        // LO: [0] and HI: [1] special registers come into play here
        std::array<i64, 2> mulDivStorage;
    private:
        std::shared_ptr<mio::MemoryPipe> observer;

        // Class that provides CPU code execution functionality
        std::unique_ptr<EeExecutor> executor;
        u8 irqTrigger{};
    };
}
