// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <range/v3/algorithm.hpp>

#include <creeper/ee/mipsiv_cached.h>
#include <common/global.h>
#include <vm/emu_vm.h>
#include <engine/ee_core.h>

namespace cosmic::creeper::ee {
    static constexpr auto cleanPcBlock{
        (static_cast<u32>(-1) ^ (superBlockCount * 4 - 1))};
    void MipsIvInterpreter::performOp(InvokeOpInfo& func, bool deduceCycles) {
        if (func.execute) {
            func.execute(func);
            cpu->incPc();
        }
        if (func.pipe == OutOfOrder::EffectivePipeline::Mac0)
            cpu->runCycles -= func.extraCycles;
        if (deduceCycles)
            cpu->runCycles--;
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
            // Todo: May not work as expected
            if (opIterator != run.begin()) {
                if ((opIterator - 1)->infoCallable.pipe == dangerousPipe)
                    isLastABr = true;
            }
            bool isBranch{opcInside->infoCallable.pipe == dangerousPipe};
            if (isLastABr || opcInside->trackablePc != *cpu->eePc)
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
            startBlock = cached.at(block).ops;
            blockPos = (localPc32 / 4) & (superBlockCount - 1);
            // We will execute multiple blocks until we find a branch instruction
            u32 blockRequiredInstr{cached.at(block).instCount - blockPos};
            runningBlock = std::span<CachedMultiOp>(
                std::addressof(startBlock[blockPos]), blockRequiredInstr);
            cpu->chPc(localPc32);

            executedInstr += runNestedInstructions(runningBlock);
            if (executedInstr != blockRequiredInstr || executedInstr == maxInstrPerExecution)
                break;
            // We have overflowed to another block
            localPc32 += executedInstr * 4;
            block = localPc32;
        }
    }
    MipsIvInterpreter::MipsIvInterpreter(raw_reference<engine::EeMipsCore> mips) :
        engine::EeExecutor(mips) {
        lastCleaned = 0;
        memset(metrics.data(), 0, sizeof(metrics));

        for (u32 trick{}; trick < std::size(metrics); trick++) {
            metrics[trick].blockPc = metrics[0].heat = 0;
            metrics[trick].isLoaded = false;
        }
        auto vmRef{redBox->openVm()};
        vm = vmRef;

        fpu = std::ref(cpu->fpu1);
        control = std::ref(cpu->ctrl0);

        redBox->leaveVm(vmRef);
    }
    u32 MipsIvInterpreter::executeCode() {
        i64 executionPipe[1];
        u32 PCs[2];
        do {
            PCs[0] = *cpu->eePc;
            PCs[1] = PCs[0] & cleanPcBlock;
            raw_reference<BlockFrequency> chosen;
            ranges::for_each(metrics, [&](auto& met){
                if (met.blockPc == PCs[1])
                    chosen = std::ref(met);
            });
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
                    cached[chosen->blockPc] = translateBlock(cached[lastCleaned], chosen->blockPc);
                    cached.erase(lastCleaned);
                    lastCleaned = 0;
                } else {
                    CachedBlock translate32{};
                    cached[chosen->blockPc] = translateBlock(translate32, chosen->blockPc);
                }
                chosen->isLoaded = true;
                isCached = true;
            }
            if (!isCached || !chosen || !chosen->isLoaded) {
                throw AppFail("No translated block was created or found; there is a bug in the code");
            }
            runFasterBlock(PCs[0], PCs[1]);
            executionPipe[0] = cpu->runCycles;
        } while (executionPipe[0] > 0);
        return PCs[0] - PCs[1];
    }

    CachedBlock MipsIvInterpreter::translateBlock(CachedBlock& refill, u32 nextPc) {
        u32 useful[2];
        useful[1] = 0;
        refill.instCount = 0;

        const u32 adjacentPage{(nextPc & 0xfffff000) + 0x1000};
        u32 probICount{(0x1000 - (nextPc & 0x00000fff)) / 4};
        if (probICount & 1)
            ;
        // 2, 4, 6
        if (probICount < 8)
            probICount = 8;
        // Okay, this is a gamble; there's likely to be a branching instruction before 80% is complete.
        // If not, the penalty will be significant
        probICount = static_cast<u32>(probICount / 1.80);
        const u64 bucketSize{refill.ops.size()};
        if (!bucketSize || bucketSize < probICount)
            refill.ops.reserve(probICount);

        for (; (nextPc & 0xfffff000) != adjacentPage; nextPc += 4) {
            CachedMultiOp thiz{};
            useful[0] = fetchPcInst(nextPc);
            thiz.trackIndex = static_cast<u16>(useful[1]++);
            thiz.trackablePc = nextPc;
            thiz.infoCallable = execBlackBox(useful[0]);

            refill.ops.push_back(thiz);
            refill.instCount++;
        }
        return refill;
    }
    void MipsIvInterpreter::performInvalidation(u32 address) {
        u32 writtenBlock{address & cleanPcBlock};
        if (!cached.contains(writtenBlock))
            return;
        for (auto& metric : metrics) {
            if (metric.blockPc == writtenBlock)
                metric = {};
        }
        cached.erase(writtenBlock);
    }
}
