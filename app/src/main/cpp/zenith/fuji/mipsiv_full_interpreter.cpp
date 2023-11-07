
#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>
namespace zenith::fuji {
    bool MipsIVInterpreter::performOp(std::function<void()> func) {
        std::bind(func);

        mainMips.lastPC = mainMips.eePC;
        mainMips.eePC++;
        mainMips.cyclesToWaste -= 4;
        return true;
    }
    u32 MipsIVInterpreter::runNestedBlocks(CachedBlock& block) {
        raw_reference<CachedBlock> blocksRw{block};
        u32 executed{};
        for (; mainMips.cyclesToWaste > 0; executed++) {
            if (blocksRw->trackIndex >= (superBlockCount - 1) ||
                blocksRw->trackablePC != *mainMips.eePC)
                break;

            performOp(blocksRw->execute);
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
            performOp(blk->execute);
            if (blk->trackIndex < superBlockCount)
                blk = blk->nextBlock;
            eBlocks++;
        }
        return eBlocks;
    }
    void MipsIVInterpreter::runBlocks(u32 pc, CachedBlock& block) {
        u32 remainBlocks{static_cast<u32>(superBlockCount - block.trackIndex)};
        u32 rate{mainMips.cyclesToWaste / 4};

        mainMips.chPC(pc);

        if (rate >= remainBlocks) {
            runByCounter(remainBlocks, block);
        } else {
            runNestedBlocks(block);
        }
    }

    MipsIVInterpreter::MipsIVInterpreter(eeiv::EEMipsCore& mips)
        : eeiv::EEExecutor(mips) {
        cached.reserve(superBlockCount - 1);
    }
    u32 MipsIVInterpreter::executeCode() {
        u32 pc{*mainMips.eePC};
        u32 superBlock{pc & static_cast<u32>(~superBlockCount)};

        if (!cached.size()) {
            feedEntireCache(superBlock);
        } else if (cached.at(pc & superBlockCount).trackablePC != pc) {
            feedEntireCache(superBlock);
        }

        runBlocks(pc, cached.at(pc & superBlockCount));

        return 0;
    }

    void MipsIVInterpreter::feedEntireCache(u32 nextPC) {
        u16 blockId{};
        CachedBlock block{};

        cached.resize(0);
        for (; blockId <= superBlockCount; blockId++) {
            block.trackIndex = blockId;

            u32 fetchOp{fetchFromPc()};
            block.execute = decodeFunc(fetchOp);
            block.trackablePC = *mainMips.lastPC;
            nextPC = block.trackablePC;

            cached.emplace_back(block);
            if (blockId)
                cached.at(blockId - 1).nextBlock = cached[blockId];
        }
    }
}
