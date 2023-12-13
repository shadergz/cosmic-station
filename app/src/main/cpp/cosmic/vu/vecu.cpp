#include <range/v3/algorithm.hpp>

#include <vu/vecu.h>
#include <console/vm/emu_vm.h>
#include <console/backdoor.h>
namespace cosmic::vu {
    VectorUnit::VectorUnit() {
        for (u8 vifI{}; vifI < 2; vifI++)
            vifTops[vifI] = nullptr;
        // vf00 is hardwired to the vector {0.0, 0.0, 0.0, 1.0}
        VuGPRs[0].w = 1.0;
        intsRegs[0].uns = 0;
        status.isVuExecuting = false;
        clock.isDirty = true;

        ranges::fill(dataSpace.heap, static_cast<u8>(0));
        ranges::fill(instSpace.heap, static_cast<u8>(0));

        nextFlagsPipe = 0;
        cfIndex = mfIndex = 3;
    }
    void VectorUnit::pulse(u32 cycles) {
        auto interVm{redBox->openVm()};
        const i64 cpuCycles{interVm->mips->cycles};
        if (clock.isDirty) {
            clock.count = cpuCycles;
        }
        i64 cyclesToRoll{interVm->mips->cycles - clock.count};
        if (!vu1Gif.has_value() && cyclesToRoll > 0) {
            interVm->mips->cop2->clearInterlock();
        }
        redBox->leaveVm(interVm);
        while (status.isVuExecuting && cyclesToRoll > 0) {
            clock.count++;
            updateMacPipeline();
        }
    }
    void VectorUnit::updateMacPipeline() {
        // Pipelines work from bottom to top; when a pipeline is finished, the status needs to be updated
        bool updSta{};
        if ((macFlags[3] & 0xffff) != (macFlags[2] & 0xffff))
            updSta = true;
        macFlags[3] = macFlags[2];
        macFlags[2] = macFlags[1];
        macFlags[1] = macFlags[0];
        macFlags[0] = nextFlagsPipe;

        if (updSta) {
            vuf |= macFlags[mfIndex] & 0x000f ? 1 : 0;
            vuf |= macFlags[mfIndex] & 0x00f0 ? 2 : 0;
            vuf |= macFlags[mfIndex] & 0x0f00 ? 4 : 0;
            vuf |= macFlags[mfIndex] & 0xf000 ? 8 : 0;

            vuf |= (vuf & 0x3f) << 6;
        }
    }

    void VectorUnit::resetVU() {
        // Performs a software-level reset
        softwareReset();
        vuPC = 0;

        for (u8 gpr{0}; gpr < 32; gpr++) {
            if (gpr < 16)
                intsRegs[gpr].uns = 0;
            if (!gpr)
                continue;
            VuGPRs[gpr].floats[0] = 0;
            VuGPRs[gpr].floats[1] = 0;
            VuGPRs[gpr].floats[2] = 0;
            VuGPRs[gpr].floats[3] = 0;

            // VuGPRs[gpr].faster = 0;
        }

    }
    // The VUs allow for software reset; this reset does not affect the VU in such
    // a way that it returns to the Genesis state
    void VectorUnit::softwareReset() {
        // DS|IS|OS|US|SS|ZS|D |I |O |U |S |Z = 0
        vuf = 0;
        isVuBusy = isOnBranch = false;
        pipeStates[0] = 0;
        pipeStates[1] = 0;

        // Cleaning our states (these states are used to control the internal VU system)
        for (u8 sid{}; sid < 4; sid++) {
            clipFlags[sid] = 0;
            macFlags[sid] = 0;
        }
    }
}