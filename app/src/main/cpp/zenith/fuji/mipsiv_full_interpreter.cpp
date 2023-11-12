
#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>
namespace zenith::fuji {
    static constexpr auto cleanPcBlock{(static_cast<u32>(-1) ^ (MipsIVInterpreter::superBlockCount * 4 - 1))};
    void MipsIVInterpreter::performOp(InvokeOpInfo& func, bool deduceCycles) {
        if (func.execute) {
            std::invoke(func.execute, func);
        }
        if (deduceCycles) {
            mainMips.lastPC = mainMips.eePC;
            mainMips.eePC++;
            mainMips.cyclesToWaste -= 4;
        }
    }
    u32 MipsIVInterpreter::runNestedBlocks(std::span<CachedMultiOp> run) {
        u32 executed{};
        auto interOp{run.begin()};
        for (; interOp != run.end(); executed++) {
            raw_reference<CachedMultiOp> opcInside{*interOp};
            if (!mainMips.cyclesToWaste || opcInside->trackablePC != *mainMips.eePC)
                break;

            // Simulating the pipeline execution with the aim of resolving one or more instructions
            // within the same cycle
            if (interOp + 1 != run.end()) {
                raw_reference<CachedMultiOp> opcSuper{*(interOp + 1)};
                // Execute only two instructions if the operations use different pipelines
                if (opcInside->infoCallable.pipe ^ opcSuper->infoCallable.pipe) {
                    performOp(opcInside->infoCallable, false);
                    performOp(opcSuper->infoCallable);
                } else {
                    performOp(opcInside->infoCallable);
                }
            }
            interOp = std::next(interOp);
        }
        return executed;
    }

    void MipsIVInterpreter::runFasterBlock(u32 pc, u32 block) {
        auto run{cached.at(block)->ops};
        u32 blockPos{pc & (superBlockCount - 1)};

        u32 remainBlocks{static_cast<u32>(superBlockCount - run[blockPos].trackIndex)};
        u32 rate{mainMips.cyclesToWaste / 4};
        mainMips.chPC(pc);

        if (rate < remainBlocks) {
            runNestedBlocks(std::span<CachedMultiOp>{run.data(), rate});
        } else {
            runNestedBlocks(run);
        }
    }

    MipsIVInterpreter::MipsIVInterpreter(eeiv::EEMipsCore& mips)
        : eeiv::EEExecutor(mips) {
        memset(metrics.data(), 0, sizeof(metrics));
    }
    u32 MipsIVInterpreter::executeCode() {
        u32 executionPipe[1];
        u32 PCs[2];
        do {
            PCs[0] = *mainMips.eePC;
            PCs[1] = PCs[0] & cleanPcBlock;

            raw_reference<BlockFrequencyMetric> chosen;
            for (auto& met: metrics) {
                if (met.blockPC == PCs[1]) {
                    chosen = std::ref(met);
                }
            }
            bool isCached{true};
            if (!chosen) {
                // Choosing the metric with the lowest frequency number
                std::sort(metrics.begin(), metrics.end());
                chosen = std::ref(metrics[0]);
                isCached = false;
            }
            [[unlikely]] if (!isCached) {
                if (cached.contains(chosen->blockPC))
                    cached.erase(chosen->blockPC);
                chosen->blockPC = PCs[1];
                chosen->heat = 0;
                chosen->isLoaded = false;
            } else {
                chosen->heat++;
            }

            if (!chosen->isLoaded) {
                cached[chosen->blockPC] = translateBlock(chosen->blockPC);
                chosen->isLoaded = true;
                isCached = true;
            }

            if (!isCached || !chosen || !chosen->isLoaded) {
                throw AppFail("No translated block was created or found; there is a bug in the code");
            }

            runFasterBlock(PCs[0], PCs[1]);
            executionPipe[0] = mainMips.cyclesToWaste;
        } while (executionPipe[0]);

        return PCs[0] - PCs[1];
    }

    std::unique_ptr<CachedBlock> MipsIVInterpreter::translateBlock(u32 nextPC) {
        u32 useful[2];
        useful[1] = 0;
        // TODO: A good approach would be the possibility of reusing the blocks instead of recreating them before each translation
        auto translated{std::make_unique<CachedBlock>()};
        for (raw_reference<CachedMultiOp> opc : translated->ops) {
            useful[0] = fetchPcInst();

            opc->trackIndex = static_cast<u16>(useful[1]++);
            opc->trackablePC = nextPC;
            opc->infoCallable = decodeFunc(useful[0]);

            nextPC += 4;
        }
        return translated;
    }
}
