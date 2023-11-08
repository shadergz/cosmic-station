#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>
#include <eeiv/ee_assembler.h>

namespace zenith::fuji {
    IvFuji3Impl(addi) {
        *gprDest = fetched & 0xffff + *gprSrc;
    }
    IvFuji3Impl(slti) {
        *gprDest = *gprSrc < (fetched & 0xffff);
    }
    IvFuji3Impl(sw) {
        // The 16-bit signed offset is added to the contents of GPR base to form the effective address
        u32 stAddr{*gprDest + fetched & 0xffff};
        mainMips.directWrite(stAddr, *gprSrc);
    }
    // if (cond < {0, null}) ...
    IvFuji3Impl(bltzal) {
        // With the 18-bit signed instruction offset, the conditional branch range is ± 128 KBytes
        i32 jump{static_cast<i32>((fetched & 0xffff) << 2)};
        // GPR[31] ← PC + 8
        *(mainMips.GprAt<u32*>(eeiv::$ra)) = *mainMips.lastPC + 8;
        mainMips.verifyAndBranch(*gprDest < 0, jump);
    }

    IvFujiSpecialImpl(ivXor) {
        *gprDest = *gprSrc ^ *gprExt;
    }
    IvFujiSpecialImpl(slt) {
        *gprDest = *gprSrc < *gprExt;
    }
}
