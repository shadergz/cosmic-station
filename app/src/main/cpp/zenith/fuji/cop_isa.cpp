// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>

namespace zenith::fuji {
    IvFuji3Impl(tlbr) {
        auto entry{mainMips.fetchTLBFromCop(mainMips.cop0.GPRs.data())};
        mainMips.cop0.loadGPRTLB(std::ref(*entry));
    }
    IvFuji3Impl(tlbwi) {
        mainMips.setTLBByIndex();
    }

    IvFuji3Impl(eret) {
        raw_reference<eeiv::c0::CoProcessor0> c0{mainMips.cop0};
        if (c0->status.error) {
            mainMips.chPC(c0->errorPC);
            c0->status.error = false;
        } else {
            mainMips.chPC(c0->ePC);
            c0->status.exception = false;
        }
        // This will set the last PC value to PC, and the PC to PC - 4
        mainMips.chPC(mainMips.eePC--);
        mainMips.updateTlb();
    }
    IvFuji3Impl(ei) {
        mainMips.cop0.enableInt();
    }
    IvFuji3Impl(di) {
        mainMips.cop0.disableInt();
    }
}