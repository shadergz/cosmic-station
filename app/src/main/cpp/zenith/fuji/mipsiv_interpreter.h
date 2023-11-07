#pragma once
#include <vector>

#include <eeiv/ee_handler.h>
#include <common/types.h>
namespace zenith::fuji {
    enum MipsIVOpcodes {
        Addi = 0x8
    };

    struct CachedBlock {
        u16 trackIndex;
        u32 trackablePC;
        std::function<void()> execute;

        raw_reference<CachedBlock> nextBlock;
    };

    class MipsIVInterpreter : public eeiv::EEExecutor {
    public:
        static constexpr u16 superBlockCount{0xff};
        MipsIVInterpreter(eeiv::EEMipsCore& mips);
        u32 executeCode() override;
    private:
        void runBlocks(u32 pc, CachedBlock& block);
        u32 runByCounter(u32 counter, CachedBlock& block);
        u32 runNestedBlocks(CachedBlock& block);
        void feedEntireCache(u32 nextPC);

        u32 fetchFromPc();
        std::function<void()> decodeFunc(u32 opcode);
        bool performOp(std::function<void()> func);

        std::vector<CachedBlock> cached;

        void addi(u32 fetched, u32* gprDest, u32* gprSrc);
    };
}
