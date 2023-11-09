#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>

namespace zenith::fuji {
    IvFuji3Impl(tlbr) {
        auto entry{mainMips.fetchTLBFromCop(mainMips.cop0.GPRs.data())};
        mainMips.cop0.loadTlbValues(std::ref(*entry));
    }
    IvFuji3Impl(eret) {
        raw_reference<eeiv::c0::CoProcessor0> c0{mainMips.cop0};
        if (c0->status.error) {
            mainMips.chPC(c0->pRid);
            c0->status.error = false;
        } else {
            mainMips.chPC(c0->pRid);
            c0->status.exception = false;
        }
        mainMips.eePC--;
        mainMips.updateTlb();
    }
    IvFuji3Impl(ei) {
        mainMips.cop0.enableInt();
    }
    IvFuji3Impl(di) {
        mainMips.cop0.disableInt();
    }
}