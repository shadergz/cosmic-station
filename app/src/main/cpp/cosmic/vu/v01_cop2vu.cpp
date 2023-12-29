#include <vu/v01_cop2vu.h>

namespace cosmic::vu {
    MacroModeCop2::MacroModeCop2(RawReference<VectorUnit> vus[2])
        : v0(vus[0]),
        v1(vus[1]) {
        cop2il = vuIl = false;
    }
    // CFC2 is a requirement, it is a way for EE to access the V0 special registers
    u32 MacroModeCop2::cfc2(u32 special) {
        if (special < 0x10)
            if (special != 0)
                return v0->intsRegs[special].uns;
        return 0;
    }
    void MacroModeCop2::ctc2(u32 special, u32 value) {
        switch (special) {
        case 0x0 ... 0xf:
            if (special < 0x10)
                if (special != 0)
                    v0->intsRegs[special].uns = static_cast<u16>(value);
            break;
        case 0x16:
            v0->spQ.uns = value; break;
        case 0x17:
            v0->spP.uns = value; break;
        default:
            throw AppFail("Invalid VU special register index {} used with CTC2 instruction, value: {}", special, value);
        }
    }
    void MacroModeCop2::clearInterlock() {
        cop2il = vuIl = false;
    }
    bool MacroModeCop2::checkInterlock() {
        return vuIl;
    }
    bool MacroModeCop2::interlockCheck(bool isCop2) {
        if (isCop2) {
            cop2il = true;
            if (!vuIl)
                return true;
        } else {
            vuIl = true;
            if (!cop2il)
                return true;
        }
        return false;
    }
}