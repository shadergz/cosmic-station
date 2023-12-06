#pragma once
#include <map>
#include <array>

#include <fuji/fuji_common.h>
#include <engine/ee_info.h>
#define IV_FUJI_SUPER_ASM(op) IV_FUJI_OP(MipsIvInterpreter::op)

namespace cosmic::fuji {
    struct OutOfOrder {
        enum EffectivePipeline {
            InvalidOne = 0,
            Eret = 0x10,
            Cop0 = 0x12
        };
        friend EffectivePipeline operator^(EffectivePipeline dest, EffectivePipeline src) {
            return static_cast<EffectivePipeline>(static_cast<u16>(dest) ^ static_cast<u16>(src));
        }
    };
    struct InvokeOpInfo {
        Operands ops;
        OutOfOrder::EffectivePipeline pipe;
        std::function<void(InvokeOpInfo& info)> execute;
    };
    struct CachedMultiOp {
        u16 trackIndex;
        u32 trackablePC;
        InvokeOpInfo infoCallable;
    };

    struct CachedBlock {
        std::array<CachedMultiOp, 128> ops;
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
        static constexpr u32 superBlockCount{0x80};
        MipsIvInterpreter(engine::EeMipsCore& mips);
        u32 executeCode() override;
    private:
        void runFasterBlock(u32 pc, u32 block);
        u32 runNestedBlocks(std::span<CachedMultiOp> run);

        std::unique_ptr<CachedBlock> translateBlock(std::unique_ptr<CachedBlock> translated, u32 nextPC);

        u32 fetchPcInst() override;

        std::function<void(InvokeOpInfo&)> decMipsIvS(u32 opcode, InvokeOpInfo& decode);
        std::function<void(InvokeOpInfo&)> decMipsIvRegImm(u32 opcode, InvokeOpInfo& decode);
        std::function<void(InvokeOpInfo&)> decMipsIvCop0(u32 opcode, InvokeOpInfo& decode);

        InvokeOpInfo decMipsBlackBox(u32 opcode);
        void performOp(InvokeOpInfo& func, bool deduceCycles = true);

        std::array<BlockFrequencyMetric, 16> metrics;
        std::map<u32, std::unique_ptr<CachedBlock>> cached;
        u32 lastCleaned;

        IV_FUJI_OP(addi);
        IV_FUJI_OP(slti);
        IV_FUJI_OP(sw); IV_FUJI_OP(sd);
        IV_FUJI_OP(bltzal);
        IV_FUJI_OP(bgez); IV_FUJI_OP(bgezl); IV_FUJI_OP(bgezall);
        IV_FUJI_OP(mtsab); IV_FUJI_OP(mtsah);
        IV_FUJI_OP(ivSyscall);

        // Memory read functions through direct translation
        IV_FUJI_OP(lb); IV_FUJI_OP(lbu);
        IV_FUJI_OP(lh); IV_FUJI_OP(lhu);
        IV_FUJI_OP(lw); IV_FUJI_OP(lwu);
        IV_FUJI_OP(ld);

        IV_FUJI_OP(movz); IV_FUJI_OP(movn);

        IV_FUJI_OP(cache);
        IV_FUJI_OP(nop);
        IV_FUJI_OP(ivBreak);

        IV_FUJI_OP(sll); IV_FUJI_OP(srl); IV_FUJI_OP(sra);
        IV_FUJI_OP(sllv); IV_FUJI_OP(srlv); IV_FUJI_OP(srav);

        IV_FUJI_OP(mult); IV_FUJI_OP(multu);
        IV_FUJI_OP(div); IV_FUJI_OP(divu);
        IV_FUJI_OP(add); IV_FUJI_OP(addu); IV_FUJI_OP(dadd); IV_FUJI_OP(daddu);
        IV_FUJI_OP(sub); IV_FUJI_OP(subu); IV_FUJI_OP(dsub); IV_FUJI_OP(dsubu);

        IV_FUJI_OP(slt);
        IV_FUJI_OP(ivXor);
        IV_FUJI_OP(bne);

        // Instructions intrinsically related to Cop0 and TLB/Exception
        IV_FUJI_OP(tlbr);
        IV_FUJI_OP(tlbwi);
        IV_FUJI_OP(eret);
        IV_FUJI_OP(ei); IV_FUJI_OP(di);

        IV_FUJI_OP(c0mfc);
        IV_FUJI_OP(c0mtc);
        IV_FUJI_OP(copbc0tf);
    };
}
