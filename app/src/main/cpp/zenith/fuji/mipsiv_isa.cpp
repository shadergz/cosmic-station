#include <fuji/mipsiv_interpreter.h>

namespace zenith::fuji {
    IvFuji3Impl(addi) {
        *gprDest = fetched & 0xffff + *gprSrc;
    }
    IvFuji3Impl(slti) {
        *gprDest = *gprSrc < (fetched & 0xffff);
    }

    IvFujiSpecialImpl(ivXor) {
        *gprDest = *gprSrc ^ *gprExt;
    }
    IvFujiSpecialImpl(slt) {
        *gprDest = *gprSrc < *gprExt;
    }
}
