#pragma once
#include <map>
#include <array>

#include <eeiv/ee_fuji.h>
#include <eeiv/ee_info.h>
#include <common/types.h>
#include <fuji/fuji_macros.h>

#define IvFuji3Impl(op) IvFuji3(MipsIVInterpreter::op)
#define IvFujiSpecialImpl(op) IvFujiSpecial(MipsIVInterpreter::op)

namespace zenith::fuji {
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
        std::array<u8, 3> ids;
        std::array<u32*, 3> regs;
        i32 value;

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
        u32 blockPC;
        u32 heat;
        bool isLoaded;

        bool operator<(const BlockFrequencyMetric& src) const {
            return heat < src.heat;
        }
    };

    class MipsIVInterpreter : public eeiv::EEExecutor {
    public:
        static constexpr u32 superBlockCount{0x80};
        MipsIVInterpreter(eeiv::EEMipsCore& mips);
        u32 executeCode() override;
    private:
        void runFasterBlock(u32 pc, u32 block);
        u32 runNestedBlocks(std::span<CachedMultiOp> run);

        std::unique_ptr<CachedBlock> translateBlock(u32 nextPC);

        u32 fetchPcInst() override;
        InvokeOpInfo decodeFunc(u32 opcode);
        void performOp(InvokeOpInfo& func, bool deduceCycles = true);

        std::array<BlockFrequencyMetric, 16> metrics;
        std::map<u32, std::unique_ptr<CachedBlock>> cached;

        IvFuji3(addi);
        IvFuji3(slti);

        IvFuji3(sw); IvFuji3(sd);

        IvFuji3(bltzal);

        // Memory read functions through direct translation
        IvFuji3(lb); IvFuji3(lbu);
        IvFuji3(lh); IvFuji3(lhu);
        IvFuji3(lw); IvFuji3(lwu);
        IvFuji3(ld);

        IvFuji3(cache);
        IvFuji3(nop);
        IvFuji3(iBreak);

        IvFujiSpecial(slt);
        IvFujiSpecial(ivXor);

        // Instructions intrinsically related to Cop0 and TLB/Exception
        IvFuji3(tlbr);
        IvFuji3(tlbwi);
        IvFuji3(eret);
        IvFuji3(ei);
        IvFuji3(di);

        IvFuji3(copbcX);
    };
}
