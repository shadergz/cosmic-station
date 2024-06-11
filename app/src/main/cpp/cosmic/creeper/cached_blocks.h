#pragma once
#include <map>
#include <vector>
#include <boost/unordered_map.hpp>

#include <creeper/inst_operands.h>
#include <engine/ee_info.h>

namespace cosmic {
    namespace vm { class EmuVm; }
    namespace engine {
        class FpuCop;
        class CtrlCop;
    }
}
namespace cosmic::creeper {

    constexpr u32 superBlockCount{4096 / 4};
    struct OutOfOrder {
        enum EffectivePipeline {
            Same = 0,
            Eret = 0x10,
            Cop0 = 0x12,
            Mac0 = 0x14,
            Branch = 0x16,
            Sync = 0x18
        };
        friend EffectivePipeline operator^(EffectivePipeline dest, EffectivePipeline src) {
            return static_cast<EffectivePipeline>(static_cast<u16>(dest) ^ static_cast<u16>(src));
        }
    };
    enum InstructionExtraCycles: i16 {
        None = 0,
        Div = 37,
        Mul = 4
    };
    struct InvokeOpInfo;
    using InvokableCached = std::function<void(Operands&)>;

    struct InvokeOpInfo {
        Operands ops;
        OutOfOrder::EffectivePipeline pipe;
        InstructionExtraCycles extraCycles;
        InvokableCached execute;
    };

    struct CachedMultiOp {
        u16 trackIndex;
        u32 trackablePc;
        InvokeOpInfo infoCallable;
    };
    struct CachedBlock {
        std::vector<CachedMultiOp> ops;
        u16 instCount;
    };

    struct BlockFrequency {
        u32 blockPc;
        u32 heat;
        bool isLoaded;

        bool operator<(const BlockFrequency& src) const {
            return heat < src.heat;
        }
    };
    using EeFunc = std::function<void(Operands)>;
    struct EeOpWithSys {
        EeFunc opcodeHandler;
        const std::string opcodeFormat;
    };

    using EeMapSpecial = boost::unordered_map<engine::MipsIvSpecial, EeOpWithSys>;
    using EeRegImm = boost::unordered_map<engine::MipsRegImmOpcodes, EeOpWithSys>;
    using EeCop = boost::unordered_map<engine::MipsIvCops, EeOpWithSys>;
    using EeCopOp2 = boost::unordered_map<engine::MipsIvCopOp2, EeOpWithSys>;

    using EeCore = boost::unordered_map<engine::MipsIvOpcodes, EeOpWithSys>;

    class MipsIvInterpreter : public engine::EeExecutor {
    public:
        MipsIvInterpreter(Ref<engine::EeMipsCore> mips);
        u32 executeCode() override;
        void performInvalidation(u32 address) override;


        static void addi(Operands ops);
        static void addiu(Operands ops);
        static void lui(Operands ops);
        static void slti(Operands ops);
        static void sw(Operands ops);
        static void sd(Operands ops);
        static void bltzal(Operands ops);
        static void bgez(Operands ops);
        static void bgezl(Operands ops);
        static void bgezall(Operands ops);
        static void mtsab(Operands ops);
        static void mtsah(Operands ops);
        static void iSyscall(Operands ops);

        // Memory read functions through direct translation
        static void lb(Operands ops);
        static void lbu(Operands ops);
        static void lh(Operands ops);
        static void lhu(Operands ops);
        static void lw(Operands ops);
        static void lwu(Operands ops);
        static void ld(Operands ops);

        static void movz(Operands ops);
        static void movn(Operands ops);

        static void cache(Operands ops);
        static void nop(Operands ops);
        static void iBreak(Operands ops);

        static void sll(Operands ops);
        static void srl(Operands ops);
        static void sra(Operands ops);
        static void sllv(Operands ops);
        static void srlv(Operands ops);
        static void srav(Operands ops);

        static void mult(Operands ops);
        static void multu(Operands ops);
        static void div(Operands ops);
        static void divu(Operands ops);
        static void add(Operands ops);
        static void addu(Operands ops);
        static void dadd(Operands ops);
        static void daddu(Operands ops);
        static void sub(Operands ops);
        static void subu(Operands ops);

        static void iAnd(Operands ops);
        static void iOr(Operands ops);
        static void iXor(Operands ops);
        static void nor(Operands ops);

        static void dsub(Operands ops);
        static void dsubu(Operands ops);

        static void slt(Operands ops);
        static void ori(Operands ops);
        static void xori(Operands ops);
        static void jr(Operands ops);

        static void bne(Operands ops);

        // Instructions intrinsically related to Cop0 and TLB/Exception
        static void tlbr(Operands ops);
        static void tlbwi(Operands ops);
        static void eret(Operands ops);
        static void ei(Operands ops);
        static void di(Operands ops);

        static void c0mfc(Operands ops);
        static void c0mtc(Operands ops);
        static void copbc0tf(Operands ops);

        // Functions related to the EEs FPU
        static void fpuMadd(Operands ops);
        static void fpuAdda(Operands ops);
    private:
        inline void getOpcodeHandler(auto opcodes, auto micro,
            InvokeOpInfo& info, EeInstructionSet& set);

        void runFasterBlock(const u32 pc, u32 block);
        u32 runNestedInstructions(std::span<CachedMultiOp> run);

        CachedBlock translateBlock(CachedBlock& refill, u32 nextPc);

        u32 fetchPcInst(u32 pc) override;

        void decodeSpecial(u32 opcode, InvokeOpInfo& codes, EeInstructionSet& set);
        void decodeRegimm(u32 opcode, InvokeOpInfo& codes, EeInstructionSet& set);
        void decodeCop(u32 opcode, InvokeOpInfo& codes, EeInstructionSet& set);

        void decodeEmotion(u32 opcode, InvokeOpInfo& microCodes);
        void performOp(InvokeOpInfo& func, bool deduceCycles = true);

        std::array<BlockFrequency, 32> metrics;
        boost::unordered_map<u32, CachedBlock> cached;
        u32 lastCleaned;

        static Ref<engine::EeMipsCore> cpu;
        static Ref<vm::EmuVm> vm;
        static Ref<engine::FpuCop> fpu;
        static Ref<engine::CtrlCop> c0;

        static EeMapSpecial ivSpecial;
        static EeRegImm ivRegImm;
        static EeCop ivCop;
        static EeCopOp2 ivCopOp2;
        static EeCore ivCore;

        static u32& doReg(const Reg regId);
        static u64& do64Reg(const Reg regId);
        static i32& signedDoReg(const Reg regId);
        static i64& signedDo64Reg(const Reg regId);

        static const auto calcOffset(Operands ops, auto reg);

        static auto signedGetOffset(const Operands ops) -> i32 {
            return ops.sins & 0xffff;
        };
        static auto getOffset(const Operands ops) -> u32 {
            return ops.inst & 0xffff;
        };
    };
}
