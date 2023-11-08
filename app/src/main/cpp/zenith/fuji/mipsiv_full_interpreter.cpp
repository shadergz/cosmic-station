
#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>
namespace zenith::fuji {
    void MipsIVInterpreter::performOp(std::function<void()> executeOp) {
        if (executeOp)
            std::invoke(executeOp);

        mainMips.lastPC = mainMips.eePC;
        mainMips.eePC++;
        mainMips.cyclesToWaste -= 4;
    }
    u32 MipsIVInterpreter::runNestedBlocks(u32 block) {
        u32 executed{};
        for (; mainMips.cyclesToWaste > 0; executed++) {
            raw_reference<CachedBlock> blocksRw{cached[block++]};
            if (blocksRw->trackIndex <= superBlockCount || blocksRw->trackablePC == *mainMips.eePC) {
                performOp(blocksRw->execute);
            }
        }
        return executed;
    }

    u32 MipsIVInterpreter::runByCounter(u32 counter, u32 block) {
        auto savedPC{&mainMips.eePC};
        u32 eBlocks{};
        for (; eBlocks < counter; ) {
            raw_reference<CachedBlock> blk{cached[block++]};
            if (blk->trackablePC != *(*savedPC))
                continue;
            performOp(blk->execute);
            if (blk->trackIndex < superBlockCount)
                ;
            eBlocks++;
        }
        return eBlocks;
    }
    void MipsIVInterpreter::runBlocks(u32 pc, u32 block) {
        u32 remainBlocks{static_cast<u32>(superBlockCount - cached[block].trackIndex)};
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

        runBlocks(pc, pc & superBlockCount);

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

            cached.emplace_back(std::move(block));
        }
    }
}
