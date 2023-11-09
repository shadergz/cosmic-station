#pragma once
#include <vector>

#include <eeiv/ee_handler.h>
#include <eeiv/ee_info.h>
#include <common/types.h>

#define IvFuji3(op)\
    void op(i32 sfet, u32* gprDest, u32* gprSrc)

#define IvFujiSpecial(op)\
    void op(i32 sfet, u32* gprDest, u32* gprSrc, u32* gprExt)

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

    struct CachedOpInfo {
        u16 trackIndex;
        u32 trackablePC;
        InvokeOpInfo infoCallable;
    };

    class MipsIVInterpreter : public eeiv::EEExecutor {
    public:
        static constexpr u16 superBlockCount{0xff};
        MipsIVInterpreter(eeiv::EEMipsCore& mips);
        u32 executeCode() override;
    private:
        void runOpsFromBlock(u32 pc, u32 block);
        u32 runByCounter(u32 counter, u32 block);
        u32 runNestedBlocks(u32 block);
        void feedEntireCache(u32 nextPC);

        u32 fetchFromPc();
        InvokeOpInfo decodeFunc(u32 opcode);
        void performOp(InvokeOpInfo& func, bool deduceCycles = true);

        std::vector<CachedOpInfo> cached;

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

        IvFujiSpecial(slt);
        IvFujiSpecial(ivXor);

        // Instructions intrinsically related to C0 and TLB/Exception
        IvFuji3(tlbr);
        IvFuji3(eret);
        IvFuji3(ei);
        IvFuji3(di);
    };
}
