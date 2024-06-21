// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <common/global.h>
#include <console/virt_devices.h>

#include <ee/ee_info.h>
#include <vu/v01_cop2vu.h>
#define BIOS_ACCESS_CHECK 0
namespace cosmic::vm {
    EmuVm::EmuVm(
        std::shared_ptr<console::VirtDevices>& virtDevs,
        std::shared_ptr<gpu::ExhibitionEngine>& dsp) :
            screenEngine(dsp), emuThread(*this) {

        outside = std::make_shared<console::BackDoor>(*this);
        sharedPipe = std::make_shared<mio::MemoryPipe>(virtDevs);

        virtDevs->level2devsInit(sharedPipe);

        mips = virtDevs->eeR5900;
        iop = virtDevs->mipsIop;
        ioDma = virtDevs->iopDma;

        gsCore = virtDevs->gs;

        mpegDecoder = virtDevs->decoderMpeg12;
        vu01 = virtDevs->VUs;

        biosHigh = std::make_shared<hle::BiosPatcher>(mips);
        scheduler = std::make_shared<Scheduler>();
        intc = std::make_shared<console::IntCInfra>(*this);
        // Our way to perform interconnection between different isolated components
        dealer = std::make_unique<hle::SyscallDealer>();

        virtDevs->level3devsInit(sharedPipe, intc);

        vu01->populate(intc, sharedPipe->controller);
        gsGif = virtDevs->gif;
        sound = virtDevs->soundPu;

        status.setDesiredFrames(30);

        Optional<vu::VectorUnit> vus[]{
            vu01->vpu0Cop2,
            vu01->vpu1Dlo
        };
        mips->cop2 = std::make_unique<vu::MacroModeCop2>(vus);
        mips->timer = std::make_unique<ee::EeTimers>(scheduler, intc);

        states->dumpImage.addListener([&]{
            dumpMemoryAtClash = *states->dumpImage;
        });

        user->success("VM loaded successfully");
    }

    // [Start of BIOS, these instructions are equivalent for both IOP and EE]
    // mfc0 $k0, $t7, 0
    // nop
    // slti $at, $k0, 0x59
    // bnez $at, 0x18
    void EmuVm::startVm() {
        std::span<u8> kernelRegion{sharedPipe->solveGlobal().as<u8*>(),
            sharedPipe->controller->mapped->biosSize()};
        try {
            biosHigh->group->readBios(kernelRegion);
            biosHigh->resetBios();
#if BIOS_ACCESS_CHECK
            PipeWrite<u32>(sharedPipe, 0x1fc00000, 0xcafebabe);
#endif
            emuThread.runVm();
        } catch (const CosmicException& except) {
            if (dumpMemoryAtClash) {
                sharedPipe->controller->mapped->printMemoryImage();
            }

            std::rethrow_exception(std::current_exception());
        }
    }
    void EmuVm::stopVm() {
        emuThread.haltVm();
    }

    void EmuVm::resetVm() {
        status.clearStatus();
        scheduler->resetCycles();

        // Resetting all co-processors
        mips->resetCore();
        gsGif->resetGif();
        gsCore->resetGraphics();

        sharedPipe->controller->resetMa();
        mpegDecoder->resetDecoder();
        mips->timer->resetTimers();

        for (u8 vu{}; vu < 2; vu++)
            vu01->vifs[vu].resetVif();
        vu01->vpu0Cop2.resetVu();
        vu01->vpu1Dlo.resetVu();

        iop->resetIop();
        ioDma->resetIoDma();
        sound->resetSound();

        // iop->iopMem->controller->mapped->iopSoftClean();
    }
    void EmuVm::dealWithSyscalls() {
        hle::SyscallOrigin origin{};
        // 08: Syscall Generated unconditionally by syscall instruction
        if (mips->cop0.cause.exCode == 0x8)
            origin = hle::SysEmotionEngine;
        else if (iop->cop.cause.code == 0x8)
            origin = hle::SysIop;

        i16 call[0];
        call[0] = mips->GPRs[ee::$v1].sh[0];
        u8 mipsCall{0x8};
        if (origin == hle::SysEmotionEngine) {
            dealer->doSyscall(origin, call[0]);
            mips->eePc = mips->GPRs[31].words[0];
            mips->cop0.cause.exCode = {};

            return;
        }
        iop->handleException(mipsCall);
        iop->cop.cause.code = {};
    }
}
