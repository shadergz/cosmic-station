#include <vm/emu_thread.h>
#include <vm/emu_vm.h>

namespace cosmic::vm {
    void EmuThread::stepMips(Ref<EmuVm>& vm, u32 mips, u32 iop, u32 bus) {
        vm->mips->pulse(mips);
        vm->iop->pulse(iop);
        // DMAC runs in parallel, which could be optimized (and will be early next year)
        vm->sharedPipe->controller->pulse(bus);
        vm->mpegDecoder->update();
    }
    void EmuThread::stepVus(Ref<EmuVm>& vm, u32 mips, u32 bus) {
        // VUs can run in parallel with EE...
        for (u8 runVifs{}; runVifs < 2; runVifs++)
            vm->vu01->vifs[runVifs].update(bus);
        vm->vu01->vpu0Cop2.pulse(mips);
        vm->vu01->vpu1Dlo.pulse(mips);
    }
    void EmuThread::stepGs(Ref<EmuVm>& vm, u32 bus) {
        vm->gsGif->update(bus);
    }

    void EmuThread::runFrameLoop(Ref<EmuVm>& vm) {
        auto sched{vm->scheduler};
        while (!vm->status.get(HasFrame)) {
            u32 mipsCycles{sched->getNextCycles(Scheduler::Mips)};
            u32 busCycles{sched->getNextCycles(Scheduler::Bus)};
            u32 iopCycles{sched->getNextCycles(Scheduler::IOP)};
            sched->updateCyclesCount();

            for (u8 shift{}; shift < 3; shift++) {
                switch (sched->affinity >> (shift * 4) & 0xf) {
                case EmotionEngine:
                    stepMips(vm, mipsCycles, iopCycles, busCycles);
                    break;
                case GS:
                    stepGs(vm, busCycles);
                    break;
                case VUs:
                    stepVus(vm, mipsCycles, busCycles);
                    break;
                }
            }
            sched->runTasks();
            // Todo: Just for testing purposes
            vm->status.markStepsDone();
            vm->status.set(HasFrame, true);
        }
    }
}