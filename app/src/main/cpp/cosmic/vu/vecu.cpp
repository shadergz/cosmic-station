#include <range/v3/algorithm.hpp>

#include <vu/vecu.h>
#include <vm/emu_vm.h>
#include <console/backdoor.h>
namespace cosmic::vu {
    VuIntPipeline::VuIntPipeline() {
        pipeline[0].clearEntry();

        pipeCurrent = 0;
    }
    void VuIntPipeline::pushInt(u8 ir, VuIntReg old, bool rw) {
        pipeline[pipeCurrent].originalValue = old;
        pipeline[pipeCurrent].affectedIr = ir;
        pipeline[pipeCurrent].rw = rw;
    }
    void VuIntPipeline::update() {
        pipeline[0] = pipeline[pipeCurrent];
        for (u8 pi{1}; pi < pipeline.size(); pi++)
            pipeline.at(pi) = pipeline.at(pi - 1);
        pipeline[pipeCurrent].clearEntry();
    }
    void VuIntPipeline::flush() {
        for (auto& pipe : pipeline) {
            pipe.clearEntry();
        }
    }

    VectorUnit::VectorUnit() {
        for (u8 vifI{}; vifI < 2; vifI++)
            vifTops[vifI] = nullptr;
        // vf00 is hardwired to the vector {0.0, 0.0, 0.0, 1.0}
        VuGPRs[0].w = 1.0;
        intsRegs[0].uns = 0;
        status.isVuExecuting = false;
        clock.isDirty = false;
        clock.count = 0;

        ranges::fill(dataSpace.heap, static_cast<u8>(0));
        ranges::fill(instSpace.heap, static_cast<u8>(0));

        nextFlagsPipe = 0;
        cfIndex = mfIndex = 3;
    }
    void VectorUnit::pulse(u32 cycles) {
        auto interVm{redBox->openVm()};
        const i64 cpuCycles{interVm->mips->cycles[0]};
        i64 cyclesToRoll{cpuCycles - clock.count};
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
            constexpr u16 vufScale2[]{1, 2, 4, 8};
            for (u8 fs{}; fs < 4; fs++)
                if (macFlags[mfIndex] & (0xf << fs * 4))
                    vuf |= static_cast<u16>(vufScale2[fs]);

            vuf |= (vuf & 0x3f) << 6;
        }
    }

    void VectorUnit::resetVU() {
        // Performs a software-level reset
        softwareReset();
        vuPc = 0;
        spI.hd = 0.f;
        spQ.hd = 0.f;
        spR.hd = 0.f;
        spP.hd = 0.f;

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
    void VectorUnit::ctc(u32 index, u32 value) {
        if (index < 0x10) {
            intsRegs[index].uns = static_cast<u16>(value);
            return;
        }
        switch (index) {
        case 20: spR.uns = value; break;
        case 21: spI.uns = value; break;
        case 28:
            if (value & 2)
                softwareReset();
            if (value & 0x200)
                ;
            break;
        }
    }
    u32 VectorUnit::cfc(u32 index) {
        if (index < 0x10)
            return intsRegs[index].uns;
        switch (index) {
        case 16: return vuf;
        case 18: return macFlags[mfIndex] & 0xffff;
        case 21: return spI.uns;
        case 22: return spQ.uns;
        case 26: return vuPc / 8;
        }
        return {};
    }
    void VectorUnit::pushIntPipe(u8 ir, u8 fir) {
        if (ir > 0xf || fir > 0xf)
            ;
        intPipeline.pushInt(ir, intsRegs[ir], ir == fir);
    }
}