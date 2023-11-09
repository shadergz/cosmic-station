
#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>
namespace zenith::fuji {
    void MipsIVInterpreter::performOp(InvokeOpInfo& func, bool deduceCycles) {
        if (func.execute) {
            std::invoke(func.execute, func);
        }

        [[likely]] if (deduceCycles) {
            mainMips.lastPC = mainMips.eePC;
            mainMips.eePC++;
            mainMips.cyclesToWaste -= 4;
        }
    }
    u32 MipsIVInterpreter::runNestedBlocks(u32 block) {
        u32 executed{};
        for (; mainMips.cyclesToWaste > 0; executed++) {
            raw_reference<CachedOpInfo> opInfo{cached[block++]};
            if (opInfo->trackIndex >= superBlockCount || opInfo->trackablePC != *mainMips.eePC)
                break;

            // Simulating the pipeline execution with the aim of resolving one or more instructions within the same cycle
            if (block < superBlockCount) {
                raw_reference<CachedOpInfo> innerRw{cached[block++]};
                // Execute only two instructions if the operations use different pipelines
                if (opInfo->infoCallable.pipe ^ innerRw->infoCallable.pipe) {
                    performOp(opInfo->infoCallable, false);
                    performOp(innerRw->infoCallable);
                } else {
                    performOp(innerRw->infoCallable);
                    block--;
                }
            } else {
                performOp(opInfo->infoCallable);
            }
        }
        return executed;
    }

    u32 MipsIVInterpreter::runByCounter(u32 counter, u32 block) {
        auto savedPC{&mainMips.eePC};
        u32 eeOps{};
        for (; eeOps < counter; ) {
            raw_reference<CachedOpInfo> opInfo{cached[block++]};
            if (opInfo->trackablePC != *(*savedPC))
                continue;
            performOp(opInfo->infoCallable);
            if (opInfo->trackIndex < superBlockCount)
                ;
            eeOps++;
        }
        return eeOps;
    }
    void MipsIVInterpreter::runOpsFromBlock(u32 pc, u32 block) {
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
        cached.reserve(superBlockCount);
    }
    u32 MipsIVInterpreter::executeCode() {
        u32 pc{*mainMips.eePC};
        u32 superBlock{pc & static_cast<u32>(~superBlockCount)};

        if (!cached.size()) {
            feedEntireCache(superBlock);
        } else if (cached.at(pc & superBlockCount).trackablePC != pc) {
            feedEntireCache(superBlock);
        }

        runOpsFromBlock(pc, pc & superBlockCount);

        return 0;
    }

    void MipsIVInterpreter::feedEntireCache(u32 nextPC) {
        u16 blockId{};
        CachedOpInfo opData{};

        cached.resize(0);
        for (; blockId <= superBlockCount; blockId++) {
            opData.trackIndex = blockId;

            u32 fetchOp{fetchFromPc()};
            opData.infoCallable = decodeFunc(fetchOp);
            opData.trackablePC = *mainMips.lastPC;
            nextPC = opData.trackablePC;

            cached.emplace_back(std::move(opData));
        }
    }
}
