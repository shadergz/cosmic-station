#pragma once

#include <span>

#include <os/neon_simd.h>
#include <mio/mmu_tlb.h>
#include <mio/mem_pipe.h>

#include <ee/ee_info.h>

#include <ee/cop0.h>
#include <ee/cop1_fu.h>
#include <ee/ee_timers.h>
#include <vu/v01_cop2vu.h>
namespace cosmic::ee {
    class EePc {
    public:
        EePc() = default;
        EePc(u32 pc) : pcValue(pc) {}
        auto operator++([[maybe_unused]] i32 inc) {
            return pcValue += 4;
        }
        auto operator--([[maybe_unused]] i32 inc) {
            return pcValue -= 4;
        }
        operator u32() const {
            return pcValue;
        }
        u32 pcValue{};
    };
    enum ExecutionMode : u8 {
        // JIT compiler, the fastest option but with various interpretation issues
        JitRe,
        // Increases instruction decoding speed through cached blocks, which is faster
        // than a simple interpreter
        CachedInterpreter
    };

    class EeMipsCore {
        static constexpr u8 countOfGPRs{32};
    public:
        EeMipsCore(std::shared_ptr<mio::MemoryPipe>& pipe);
        ~EeMipsCore();

        void resetCore();
        void pulse(u32 cycles);
        u32 fetchByPc();
        u32 fetchByAddress(u32 address);
        void invalidateExecRegion(u32 address);

        u64 writeArr(u32 address, std::span<u32> dataBlk);
        struct ValidatePtr {
            std::uintptr_t firstPage{1};
            template <typename T>
            auto operator ==(const T& check) const {
                return reinterpret_cast<std::uintptr_t>(check) == firstPage;
            }
            template <typename T>
            auto operator<=(const T& check) const {
                return reinterpret_cast<std::uintptr_t>(check) <= firstPage;
            }
        };
        ValidatePtr pagerChecker;
        template <typename T>
        T mipsRead(u32 address) {
            if (address) {
            }
            const u32 virtAddrSpace{address / 4096};
            const auto virtPagedMemory{cop0.virtMap[virtAddrSpace]};

            if (pagerChecker == virtPagedMemory) {
                if constexpr (sizeof(T) == 4) {
                    return PipeRead<T>(memPipe, address & 0x1fffffff);
                }
                return {};
            }
            if (pagerChecker <= virtPagedMemory)
                return {};
            T fetchedData;
            std::memcpy(&fetchedData, &virtPagedMemory[address & 0xfff], sizeof(T));
            return fetchedData;
        }
        template<typename T>
        void mipsWrite(u32 address, T value) {
            if (!address) {
            }
            const u32 virtAddrSpace{address / 4096};
            const auto virtPagedMemory{cop0.virtMap[virtAddrSpace]};

            if (pagerChecker == virtPagedMemory) {
                if constexpr (sizeof(T) == 4) {
                    PipeWrite<T>(memPipe, address & 0x1fffffff, value);
                }
                return;
            }
            if (pagerChecker <= virtPagedMemory)
                return;
            std::memcpy(&virtPagedMemory[address & 0xfff], &value, sizeof(T));
            invalidateExecRegion(address);
        }

        inline u32 incPc() {
            chPc(eePc);
            eePc++;
            return lastPc;
        }
        inline void chPc(u32 neoPC) {
            lastPc = eePc;
            isABranch = true;
            eePc = neoPC;
        }
        i64 getHtzCycles(bool total) const;
        void printStates();
        void branchByCondition(bool cond, i32 jumpRel);
        void branchOnLikely(bool cond, i32 jumpRel);

        mio::TlbPageEntry& fetchTlbFromCop(u32 c0Regs[]);
        void setTlbByIndex();

        void handleException(u8 el, u32 exceptVec, u8 code);

        void setLoHi(i64 lo, i64 hi);
        void setLoHi(u64 split);

        [[maybe_unused]] inline void haltCpu() {
            irqTrigger = 1;
            runCycles = {};
        }
        [[maybe_unused]] inline void unHaltCpu() {
            irqTrigger = {};
            if (runCycles < 0)
                runCycles = {};
        }
        bool isABranch{};
        u32 delaySlot{};
        i32 runCycles;

        ExecutionMode cpuMode{ExecutionMode::CachedInterpreter};
        CtrlCop cop0;
        FpuCop cop1;
        std::unique_ptr<vu::MacroModeCop2> cop2;

        EePc eePc{},
            lastPc{};
        std::unique_ptr<EeTimers> timer;

        union eeRegister {
            eeRegister() {}
            os::vec qw{};
            std::array<u64, 2> dw;
            std::array<i64, 2> sdw;
            std::array<u32, 4> words;
            std::array<i32, 4> swords;
            std::array<u16, 8> uh;
            std::array<i16, 8> sh;

        };
        std::array<eeRegister, countOfGPRs> GPRs;
        u32 sa;

        // LO: [0] and HI: [1] special registers come into play here
        std::array<i64, 2> mulDivStorage;
        std::array<u8, 1024 * 16> scratchPad;
    private:
        std::shared_ptr<mio::MemoryPipe> memPipe;

        // Class that provides CPU code execution functionality
        std::unique_ptr<EeExecutor> executor;
        u8 irqTrigger{};
    };
}
