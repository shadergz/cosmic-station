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
        u32 vMul{};
        switch (id) {
        case 100 ... 103:
            vMul = vu->vuGPRs[rad].uns[id - 100]; break;
        case 104:
            vMul = vu->getSpecialReg(vu::I).uns; break;
        case 105:
            vMul = vu->getSpecialReg(vu::Q).uns; break;
        default:
            vMul = vu->vuGPRs[rad].uns[idx];
        }
        const auto vBase{vu->vuGPRs[mul].uns[idx]};

        // https://fobes.dev/ps2/detecting-emu-vu-floats
        if (vMul != 1.) {
            return vu->toSony754(vMul) * vu->toSony754(vBase);
        }
        vMul &= static_cast<u32>(~(0x8000));
        return static_cast<f32>(vMul | 0x7fff);
    }
    void VuMicroInterpreter::vectorizeXyZw(
        VuMicroOperands& ops, std::function<void(u32)> vecMathCb) {

        constexpr u32 x{1 << 3};
        constexpr u32 y{1 << 2};
        constexpr u32 z{1 << 1};
        constexpr u32 w{1};

        if (ops.field & x) {
            vecMathCb(x);
        } else {
            vu->clsMacFlags(x);
        }
        if (ops.field & y) {
            vecMathCb(y);
        } else {
            vu->clsMacFlags(y);
        }
        if (ops.field & z) {
            vecMathCb(z);
        } else {
            vu->clsMacFlags(z);
        }
        if (ops.field & w) {
            vecMathCb(w);
        } else {
            vu->clsMacFlags(w);
        }
    }
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

