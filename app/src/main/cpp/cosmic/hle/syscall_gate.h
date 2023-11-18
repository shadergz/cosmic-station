#pragma once

#include <common/types.h>
#include <console/backdoor.h>
#include <eeiv/ee_info.h>
namespace cosmic::hle {
    enum CallParams {
        Return = eeiv::$v0,
        Param0 = eeiv::$a0,
        Param1 = eeiv::$a1,
        Param2 = eeiv::$a2,
        Param3 = eeiv::$a3,
        Param4 = eeiv::$t0,
        Param5 = eeiv::$t1
    };

    enum SyscallOrigin {
        SysEmotionEngine,
        SysIop
    };
    class SyscallDealer {
    public:
        void doSyscall(SyscallOrigin origin, i16 sys);
    private:
        void hleResetEE(raw_reference<console::EmuVM> vm);
    };
}
