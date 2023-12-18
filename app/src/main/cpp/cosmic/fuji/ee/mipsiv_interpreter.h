#pragma once
#include <map>
#include <vector>

#include <fuji/fuji_ops.h>
#include <engine/ee_info.h>
namespace cosmic::fuji::ee {
    constexpr u32 superBlockCount{4096 / 4};
    struct OutOfOrder {
        enum EffectivePipeline {
            InvalidOne = 0,
            Eret = 0x10,
            Cop0 = 0x12,
            Mac0 = 0x14,
            Branch = 0x16,
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
    struct InvokeOpInfo {
        Operands ops;
        OutOfOrder::EffectivePipeline pipe;
        InstructionExtraCycles extraCycles;
        std::function<void(InvokeOpInfo& info)> execute;
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

    struct BlockFrequencyMetric {
        u32 blockPc;
        u32 heat;
        bool isLoaded;

        bool operator<(const BlockFrequencyMetric& src) const {
            return heat < src.heat;
        }
    };

    class MipsIvInterpreter : public engine::EeExecutor {
    public:
        MipsIvInterpreter(raw_reference<engine::EeMipsCore> mips);
        u32 executeCode() override;
        void performInvalidation(u32 address) override;
    private:
        void runFasterBlock(const u32 pc, u32 block);
        u32 runNestedInstructions(std::span<CachedMultiOp> run);

        CachedBlock translateBlock(CachedBlock& refill, u32 nextPc);

        u32 fetchPcInst(u32 pc) override;

        std::function<void(InvokeOpInfo&)> decMipsIvS(u32 opcode, InvokeOpInfo& decode);
        std::function<void(InvokeOpInfo&)> decMipsIvRegImm(u32 opcode, InvokeOpInfo& decode);
        std::function<void(InvokeOpInfo&)> decMipsIvCop0(u32 opcode, InvokeOpInfo& decode);

        InvokeOpInfo decMipsBlackBox(u32 opcode);
        void performOp(InvokeOpInfo& func, bool deduceCycles = true);

        std::array<BlockFrequencyMetric, 32> metrics;
        std::map<u32, CachedBlock> cached;
        u32 lastCleaned;

        void addi(Operands ops);
        void slti(Operands ops);
        void sw(Operands ops); void sd(Operands ops);
        void bltzal(Operands ops);
        void bgez(Operands ops); void bgezl(Operands ops); void bgezall(Operands ops);
        void mtsab(Operands ops); void mtsah(Operands ops);
        void ivSyscall(Operands ops);

        // Memory read functions through direct translation
        void lb(Operands ops); void lbu(Operands ops);
        void lh(Operands ops); void lhu(Operands ops);
        void lw(Operands ops); void lwu(Operands ops);
        void ld(Operands ops);

        void movz(Operands ops); void movn(Operands ops);

        void cache(Operands ops);
        void nop(Operands ops);
        void ivBreak(Operands ops);

        void sll(Operands ops); void srl(Operands ops); void sra(Operands ops);
        void sllv(Operands ops); void srlv(Operands ops); void srav(Operands ops);

        void mult(Operands ops); void multu(Operands ops);
        void div(Operands ops); void divu(Operands ops);
        void add(Operands ops); void addu(Operands ops); void dadd(Operands ops); void daddu(Operands ops);
        void sub(Operands ops); void subu(Operands ops); void dsub(Operands ops); void dsubu(Operands ops);

        void slt(Operands ops);
        void ivXor(Operands ops);
        void bne(Operands ops);

        // Instructions intrinsically related to Cop0 and TLB/Exception
        void tlbr(Operands ops);
        void tlbwi(Operands ops);
        void eret(Operands ops);
        void ei(Operands ops);
        void di(Operands ops);

        void c0mfc(Operands ops);
        void c0mtc(Operands ops);
        void copbc0tf(Operands ops);
    };
}
