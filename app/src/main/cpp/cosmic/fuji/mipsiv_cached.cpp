// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <fuji/mipsiv_interpreter.h>
#include <engine/ee_core.h>
namespace cosmic::fuji {
    static constexpr auto cleanPcBlock{
        (static_cast<u32>(-1) ^ (superBlockCount * 4 - 1))};
    void MipsIvInterpreter::performOp(InvokeOpInfo& func, bool deduceCycles) {
        if (func.execute) {
            std::invoke(func.execute, func);
        }
        if (deduceCycles) {
            mainMips.chPC(*mainMips.eePc + 4);
            mainMips.cyclesToWaste -= 4;
        }
    }
    u32 MipsIvInterpreter::runNestedInstructions(std::span<CachedMultiOp> run) {
        static const auto dangerousPipe{OutOfOrder::EffectivePipeline::Branch};
        static const auto invPipe{OutOfOrder::EffectivePipeline::InvalidOne};
        u32 executedInst{};
        auto opIterator{std::begin(run)};
        auto endIterator{std::end(run)};

        for (; opIterator != run.end(); executedInst++) {
            raw_reference<CachedMultiOp> opcInside{*opIterator};
            bool isLastABr{false};
            if (opIterator != run.begin()) {
                if ((opIterator - 1)->infoCallable.pipe == dangerousPipe)
                    isLastABr = true;
            }
            bool isBranch{opcInside->infoCallable.pipe == dangerousPipe};
            if (isLastABr || opcInside->trackablePc != *mainMips.eePc)
                break;
            if (isBranch) {
                performOp(opcInside->infoCallable);
                break;
            }
            if ((opIterator + 1) != endIterator) {
                // Simulating the pipeline execution with the aim of resolving one or more instructions
                // within the same cycle
                raw_reference<CachedMultiOp> opcSuper{*(opIterator + 1)};
                // Execute only two instructions if the operations use different pipelines
                if ((opcInside->infoCallable.pipe ^ opcSuper->infoCallable.pipe) != invPipe) {
                    performOp(opcInside->infoCallable, false);
                    performOp(opcSuper->infoCallable);
                    opIterator = std::next(opIterator);
                    executedInst++;
                } else {
                    performOp(opcInside->infoCallable);
                }
            } else {
                performOp(opcInside->infoCallable);
            }
            opIterator = std::next(opIterator);
        }
        return executedInst;
    }

    void MipsIvInterpreter::runFasterBlock(const u32 pc, u32 block) {
        std::span<CachedMultiOp>
            startBlock{},
            runningBlock{};
        u32 localPc32{pc};
        u32 blockPos;
        u32 executedInstr{};
        constexpr u32 maxInstrPerExecution{1024};

        for (; cached.contains(block); ) {
            startBlock = cached.at(block)->ops;
            blockPos = (localPc32 / 4) & (superBlockCount - 1);
            // We will execute multiple blocks until we find a branch instruction
            u32 blockRequiredInstr{cached.at(block)->instCount - blockPos};
            runningBlock = std::span<CachedMultiOp>(
                std::addressof(startBlock[blockPos]), blockRequiredInstr);
            mainMips.chPC(localPc32);

            executedInstr += runNestedInstructions(runningBlock);
            if (executedInstr != blockRequiredInstr || executedInstr == maxInstrPerExecution)
                break;
            // We have overflowed to another block
            localPc32 += executedInstr * 4;
            block = localPc32;
        }
    }
    MipsIvInterpreter::MipsIvInterpreter(engine::EeMipsCore& mips) :
        engine::EeExecutor(mips) {
        lastCleaned = 0;
        memset(metrics.data(), 0, sizeof(metrics));

        for (u32 trick{}; trick < std::size(metrics); trick++) {
            metrics[trick].blockPc = metrics[0].heat = 0;
            metrics[trick].isLoaded = false;
        }
    }
    u32 MipsIvInterpreter::executeCode() {
        i64 executionPipe[1];
        u32 PCs[2];
        do {
            PCs[0] = *mainMips.eePc;
            PCs[1] = PCs[0] & cleanPcBlock;
            raw_reference<BlockFrequencyMetric> chosen;
            for (auto& met: metrics) {
                if (met.blockPc == PCs[1]) {
                    chosen = std::ref(met);
                    break;
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
                if (cached.contains(chosen->blockPc)) {
                    lastCleaned = chosen->blockPc;
                }
                chosen->blockPc = PCs[1];
                chosen->heat = 0;
                chosen->isLoaded = false;
            } else {
                chosen->heat++;
            }

            if (!chosen->isLoaded) {
                [[unlikely]] if (lastCleaned) {
                    cached[chosen->blockPc] = translateBlock(std::move(cached[lastCleaned]), chosen->blockPc);
                    cached.erase(lastCleaned);
                    lastCleaned = 0;
                } else {
                    std::unique_ptr<CachedBlock> transX32{std::make_unique<CachedBlock>()};
                    cached[chosen->blockPc] = translateBlock(std::move(transX32), chosen->blockPc);
                }
                chosen->isLoaded = true;
                isCached = true;
            }
            if (!isCached || !chosen || !chosen->isLoaded) {
                throw AppFail("No translated block was created or found; there is a bug in the code");
            }
            runFasterBlock(PCs[0], PCs[1]);
            executionPipe[0] = mainMips.cyclesToWaste;
        } while (executionPipe[0] > 0);
        return PCs[0] - PCs[1];
    }

    std::unique_ptr<CachedBlock> MipsIvInterpreter::translateBlock(std::unique_ptr<CachedBlock> translated, u32 nextPc) {
        u32 useful[2];
        useful[1] = 0;
        translated->instCount = 0;

        u32 adjacentPage{(nextPc & 0xfffff000) + 1};
        for (raw_reference<CachedMultiOp> opc : translated->ops) {
            if ((nextPc & 0xfffff000) == adjacentPage)
                break;

            useful[0] = fetchPcInst();
            opc->trackIndex = static_cast<u16>(useful[1]++);
            opc->trackablePc = nextPc;
            opc->infoCallable = decMipsBlackBox(useful[0]);

            nextPc += 4;
            translated->instCount++;
        }
        return translated;
    }
}
