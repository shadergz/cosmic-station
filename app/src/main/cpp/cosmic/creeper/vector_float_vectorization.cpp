#include <creeper/vector_codes.h>
#include <vu/vecu.h>
namespace cosmic::creeper {
    void VuMicroInterpreter::mr32(VuMicroOperands& ops) {
        static const std::array<u8, 4> rotates{8, 4, 2, 1};
        f32 fsX{vu->vuGPRs[ops.fs].floats[0]};

        const i32 x{ops.fd & rotates[0]};
        const i32 y{ops.fd & rotates[1]};
        const i32 z{ops.fd & rotates[2]};
        const i32 w{ops.fd & rotates[3]};

        if (x)
            vu->vuGPRs[ops.ft].floats[0] = vu->vuGPRs[ops.fs].floats[1];
        if (y)
            vu->vuGPRs[ops.ft].floats[1] = vu->vuGPRs[ops.fs].floats[2];
        if (z)
            vu->vuGPRs[ops.ft].floats[2] = vu->vuGPRs[ops.fs].floats[3];
        if (w)
            vu->vuGPRs[ops.ft].floats[3] = fsX;
    }
    f32 VuMicroInterpreter::fasterPs2VuMul(Reg rad, u32 id, Reg mul, u32 idx) {
        f32 vmuli{};
        switch (id) {
        case 100 ... 103:
            vmuli = vu->vuGPRs[rad].floats[id - 100]; break;
        case 104:
            vmuli = vu->getSpecialReg(vu::I).hd; break;
        case 105:
            vmuli = vu->getSpecialReg(vu::Q).hd; break;
        default:
            vmuli = vu->vuGPRs[rad].floats[idx];
        }
        const f32 vbase{vu->vuGPRs[mul].floats[idx]};

        // https://fobes.dev/ps2/detecting-emu-vu-floats
        [[likely]] if (
                vmuli != 1. ||
                vbase != 1.) {
            return vu->toSony754(static_cast<u32>(vmuli)) *
                   vu->toSony754(static_cast<u32>(vbase));
        }
        return {};
    }
    std::function<void(VuMicroOperands&,
        std::function<void(u32)>)> VuMicroInterpreter::vectorizeXyZw{
            [](VuMicroOperands& ops, std::function<void(u32 xYzW)> callback) {
        for (u32 xYzW{}; xYzW < 4; xYzW++) {
            if (ops.field & (1 << (3 - xYzW))) {
                vu->clsMacFlags(xYzW);
                continue;
            }
            callback(xYzW);
        }
    }};

    // Those instruction is applicable to both VU0 and VU1
    void VuMicroInterpreter::mul(VuMicroOperands& ops) {
        vectorizeXyZw(ops, [&](u32 xYzW){
            vu->vuGPRs[ops.fd].floats[xYzW] = vu->modifierMacFlags(
                fasterPs2VuMul(ops.ft, 0, ops.fs, xYzW), xYzW);
        });
    }
    void VuMicroInterpreter::mula(VuMicroOperands& ops) {
        vectorizeXyZw(ops, [&](u32 xYzW){
            vu->acc.floats[xYzW] = vu->modifierMacFlags(
                fasterPs2VuMul(ops.ft, 0, ops.fs, xYzW), xYzW);
        });
    }
    void VuMicroInterpreter::mulabc(VuMicroOperands& ops) {
        vectorizeXyZw(ops, [&](u32 xYzW){
            vu->acc.floats[xYzW] = vu->modifierMacFlags(
                fasterPs2VuMul(ops.ft, 100 + ops.bc, ops.fs, xYzW), xYzW);
        });
    }
    void VuMicroInterpreter::mulai(VuMicroOperands& ops) {
        vectorizeXyZw(ops, [&](u32 xYzW){
            vu->acc.floats[xYzW] = vu->modifierMacFlags(
                fasterPs2VuMul(ops.ft, 104, ops.fs, xYzW), xYzW);
        });
    }
    void VuMicroInterpreter::mulaq(VuMicroOperands& ops) {
        vectorizeXyZw(ops, [&](u32 xYzW){
            vu->acc.floats[xYzW] = vu->modifierMacFlags(
                fasterPs2VuMul(ops.ft, 105, ops.fs, xYzW), xYzW);
        });
    }
}

