#pragma once
#include <vector>

#include <eeiv/ee_handler.h>
#include <common/types.h>

#define IvFuji3(op)\
    void op(u32 fetched, u32* gprDest, u32* gprSrc)

#define IvFujiSpecial(op)\
    void op(u32 fetched, u32* gprDest, u32* gprSrc, u32* gprExt)

#define IvFuji3Impl(op) IvFuji3(MipsIVInterpreter::op)
#define IvFujiSpecialImpl(op) IvFujiSpecial(MipsIVInterpreter::op)

namespace zenith::fuji {
    enum MipsIVOpcodes {
        SpecialOpcodes = 0x0,
        Addi = 0x8,
        Slti = 0xa,
        Bltzal = 0x10,
        SpecialSlt = 0x2a,
        Sw = 0x2b,
        SpecialXor = 0x26,
    };

    struct CachedBlock {
        u16 trackIndex;
        u32 trackablePC;
        std::function<void()> execute;
    };

    class MipsIVInterpreter : public eeiv::EEExecutor {
    public:
        static constexpr u16 superBlockCount{0xff};
        MipsIVInterpreter(eeiv::EEMipsCore& mips);
        u32 executeCode() override;
    private:
        void runBlocks(u32 pc, u32 block);
        u32 runByCounter(u32 counter, u32 block);
        u32 runNestedBlocks(u32 block);
        void feedEntireCache(u32 nextPC);

        u32 fetchFromPc();
        std::function<void()> decodeFunc(u32 opcode);
        void performOp(std::function<void()> func);

        std::vector<CachedBlock> cached;

        IvFuji3(addi);
        IvFuji3(slti);
        IvFuji3(sw);
        IvFuji3(bltzal);

        IvFujiSpecial(ivXor);
        IvFujiSpecial(slt);
    };
}
