#include <range/v3/algorithm.hpp>

#include <vu/vecu.h>
#include <vm/emu_vm.h>
#include <console/backdoor.h>
#include <common/global.h>
#include <cosmic/creeper/vector_codes.h>
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

    VectorUnit::VectorUnit(Wrapper<VectorUnit> vu2, VuWorkMemory vuWm) :
        paraVu(vu2),
        vecRegion(vuWm) {

        for (u8 vifI{}; vifI < 2; vifI++)
            vifTops[vifI] = {};
        exe = std::make_unique<creeper::VuMicroInterpreter>(*this);

        // vf00 is hardwired to the vector {0.0, 0.0, 0.0, 1.0}
        vuGPRs[0].w = 1.0;
        intsRegs[0].uns = 0;
        status.isVuExecuting = false;
        clock.isDirty = false;
        clock.count = clock.runCycles = clock.trigger = 0;
        vu1Gif = {};

        ranges::fill(vecRegion.rw, static_cast<u8>(0));
        ranges::fill(vecRegion.re, static_cast<u8>(0));

        cachedQ.uns = 0;
        cachedP.uns = 0;

        nextFlagsPipe = 0;
        cfIndex = mfIndex = 3;
    }
    void VectorUnit::pulse(u32 cycles) {
        if (!ee && outside) {
            auto interVm{outside->openVm()};
            ee = interVm->mips;
            outside->leaveVm(interVm);
        }

        i64 cyclesHigh;
        if (clock.trigger < 0) {
            clock.trigger += cycles;
            return;
        }
        i64 eePulses{ee->getHtzCycles(false)};
        if (eePulses > 0)
            return;
        cyclesHigh = std::abs(eePulses);
        const auto isVu0{!vu1Gif.has_value()};

        if (isVu0 && cyclesHigh) {
            ee->cop2->clearInterlock();
        }
        updateDeltaCycles(cyclesHigh);

        for (; status.isVuExecuting && clock.runCycles--; ) {
            updateDeltaCycles(1, true);

            updateMacPipeline();
            updateDivEfuPipes();
            intPipeline.update();

            if (path1.stallXgKick) {
                stallByXgKick();
                break;
            }
            exe->executeCode();
        }
        if (path1.transferringGif) {
            i64 cycles2Gif{path1.cycles + clock.runCycles};
            if (!cycles2Gif)
                return;
            path1.cycles = static_cast<u32>(cycles2Gif);
            startXgKick2Gif();
        }

        if (!status.isVuExecuting) {
            if (ee->getHtzCycles(true) >= clock.count)
                return;
            if (isVu0) {
            }
        }
    }
    void VectorUnit::updateDeltaCycles(i64 add, bool incCount) {
        if (incCount) {
            clock.count += add;
            return;
        }
        clock.runCycles += add;
        clock.trigger -= clock.runCycles;
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

    void VectorUnit::resetVu() {
        // Performs a software-level reset
        softwareReset();
        vuPc = 0;
        spI.hd = 0.f;
        spQ.hd = 0.f;
        spR.hd = 0.f;
        spP.hd = 0.f;
        acc.faster = {};

        for (u8 gpr{0}; gpr < 32; gpr++) {
            if (gpr < 16)
                intsRegs[gpr].uns = 0;
            if (!gpr)
                continue;
            vuGPRs[gpr].floats[0] = 0;
            vuGPRs[gpr].floats[1] = 0;
            vuGPRs[gpr].floats[2] = 0;
            vuGPRs[gpr].floats[3] = 0;
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
                if (paraVu)
                    paraVu->softwareReset();
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
    void VectorUnit::establishVif(u16 conTops[2], Wrapper<gs::GifBridge> gif) {
        for (u8 top{}; top < 2; top++)
            vifTops[top] = Wrapper(conTops[top]);

        if (gif)
            vu1Gif = gif;
    }
    void VectorUnit::pushIntPipe(u8 ir, u8 fir) {
        if (ir > 0xf || fir > 0xf) {
        }
        intPipeline.pushInt(ir, intsRegs[ir], ir == fir);
    }
    void VectorUnit::startProgram(u32 addr) {
        const u32 start{addr & getMemMask()};
        const u32 oldPc{vuPc};

        if (!status.isVuExecuting) {
            status.isVuExecuting = true;
            vuPc = start;

            // Resets the state of the pipeline; this operation
            // will flush all pending data from the pipeline
            propagateUpdates();
        }
        const i32 vuId{paraVu ? 1 : 0};

        user->success("(VU{}) vcallms executed, previous microprogram at {}, new program at {}",
            vuId, oldPc, vuPc);
    }
    u32 VectorUnit::getMemMask() const noexcept {
        u32 mask{};
        if (paraVu && vecRegion.re.size() == 1024 * 4 * 4)
            mask = 0x3fff;
        else if (!vu1Gif)
            mask = 0xfff;
        return mask;
    }
    void VectorUnit::stopProgram() {
        if (status.isVuExecuting) {
        }
        status.isVuExecuting = false;
    }
    void VectorUnit::propagateUpdates() {
        for (i64 pipe{}; pipe < 4; pipe++)
            updateMacPipeline();

        intPipeline.flush();
        status.div = SpecialVuEvent{false, clock.count};
        status.efu = SpecialVuEvent{false, clock.count};

        spQ = cachedQ;
        spP = cachedP;
    }
    void VectorUnit::updateDivEfuPipes() {
        bool divWc,
            efuWc;

        divWc = clock.count >= status.div.finishAfter;
        efuWc = clock.count >= status.efu.finishAfter;
        if (status.div.isStarted && divWc) {
            status.div.isStarted = false;
            spQ = cachedQ;
        }
        if (status.efu.isStarted && efuWc) {
            status.efu.isStarted = false;
            spP = cachedP;
        }
    }
    u32 VectorUnit::fetchByPc() {
        auto microCode{reinterpret_cast<u32*>(vecRegion.re.data())};
        u32 isPcDangled{vuPc & 1};
        if (isPcDangled) {
        }
        u32 opcode{microCode[vuPc / 4]};
        vuPc += 4;
        return opcode;
    }

    f32 VectorUnit::modifierMacFlags(const f32 val, u32 index) {
        auto treat{static_cast<u32>(val)};
        const auto flagId{3 - index};

        // Test the sign bit of the float, we need to set or clear the sign flag
        if (treat & 0x8000'0000)
            nextFlagsPipe |= 0x10 << flagId;
        else // Cleaning
            nextFlagsPipe &= ~(0x10 << flagId);

        // Zero, Clear Underflow and Overflow
        if ((treat & 0x7fff'ffff) == 0) {
            nextFlagsPipe |= 1 << flagId;
            nextFlagsPipe &= ~(0x1100 << flagId);

            return val;
        }
        // https://github.com/PSI-Rockin/DobieStation/blob/master/src/core/ee/vu.cpp#L1023
        switch ((treat >> 23) & 0xff) {
        case 0: // Underflow, set zero
            nextFlagsPipe |= 0x101 << flagId;
            nextFlagsPipe |= ~(0x1000 << flagId);
            treat &= 0x8000'0000;
            break;
        case 255: // Overflow
            nextFlagsPipe |= 0x1000 << flagId;
            nextFlagsPipe &= ~(0x101 << flagId);
            treat &= 0x8000'0000;
            treat |= 0x7f7f'ffff;
            break;
        default: // Clear all but sign
            nextFlagsPipe &= ~(0x1101 << flagId);
        }
        return static_cast<f32>(treat);
    }
    void VectorUnit::clsMacFlags(u32 index) {
        nextFlagsPipe &= ~(0x1111 << (3 - index));
    }
}