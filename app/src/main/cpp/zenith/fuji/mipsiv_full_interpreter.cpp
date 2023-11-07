
#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>
namespace zenith::fuji {
    u32 MipsIVInterpreter::runNestedBlocks(CachedBlock& block) {
        raw_reference<CachedBlock> blocksRw{block};

        u32 executed{};
        for (; mainMips.cyclesToWaste > 0; executed++) {
            if (blocksRw->trackIndex  >= superBlockCount || blocksRw->trackablePC != *mainMips.eePC)
                break;

            std::bind(blocksRw->execute);
            blocksRw = blocksRw->nextBlock;
        }
        return executed;
    }

    u32 MipsIVInterpreter::runByCounter(u32 counter, CachedBlock& block) {
        raw_reference<CachedBlock> blk{block};
        auto savedPC{&mainMips.eePC};
        u32 eBlocks{};

        for (; eBlocks < counter; ) {
            if (blk->trackablePC != *(*savedPC))
                continue;
            std::bind(blk->execute);
            if (blk->trackIndex < superBlockCount)
                blk = blk->nextBlock;

            eBlocks++;
        }
        return eBlocks;
    }
    void MipsIVInterpreter::runBlocks(u32 pc, CachedBlock& block) {
        u8 remainBlocks{static_cast<u8>(superBlockCount - block.trackIndex)};
        u32 rate{mainMips.cyclesToWaste / 4};

        if (rate <= remainBlocks) {
            runByCounter(remainBlocks, block);
        } else {
            runNestedBlocks(block);
        }
    }

    MipsIVInterpreter::MipsIVInterpreter(eeiv::EEMipsCore& mips)
        : eeiv::EEExecutor(mips) {
        cached.clear();
    }

    u32 MipsIVInterpreter::executeCode() {
        u32 pc{*mainMips.eePC};
        u32 superBlock{pc & static_cast<u32>(~(superBlockCount - 1))};
        if (!cached.contains(superBlock)) {
            feedEntireCache(superBlock);
        }
        runBlocks(pc, cached.at(pc));

        return 0;
    }

    void MipsIVInterpreter::feedEntireCache(u32 nextPC) {
        u16 blockId{1};
        CachedBlock block{};

        cached.clear();
        for (; blockId < superBlockCount; blockId++) {
            block.trackIndex = blockId;

            u32 fetchOp{fetchFromPc()};
            block.execute = decodeFunc(fetchOp);
            block.trackablePC = *mainMips.lastPC;

            cached[nextPC] = block;
            if (blockId)
                cached[nextPC - 4].nextBlock = cached[nextPC];
        }
    }
}
