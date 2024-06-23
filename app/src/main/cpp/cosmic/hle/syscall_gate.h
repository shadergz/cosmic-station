#pragma once

#include <common/types.h>
#include <console/backdoor.h>
#include <ee/ee_info.h>
namespace cosmic::hle {
    enum CallParams {
        Return = ee::$v0,
        Param0 = ee::$a0,
        Param1 = ee::$a1,
        Param2 = ee::$a2,
        Param3 = ee::$a3,
        Param4 = ee::$t0,
        Param5 = ee::$t1
    };

    enum SyscallOrigin {
        SysEmotionEngine,
        SysIop
    };
    class SyscallDealer {
    public:
        SyscallDealer();
        void doSyscall(SyscallOrigin origin, i16 sys);
    private:
        void resetEe();
        Wrapper<vm::EmuVm> vm;
    };
}
