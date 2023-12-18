#pragma once

#include <common/types.h>
#include <console/backdoor.h>
#include <engine/ee_info.h>
namespace cosmic::hle {
    enum CallParams {
        Return = engine::$v0,
        Param0 = engine::$a0,
        Param1 = engine::$a1,
        Param2 = engine::$a2,
        Param3 = engine::$a3,
        Param4 = engine::$t0,
        Param5 = engine::$t1
    };

    enum SyscallOrigin {
        SysEmotionEngine,
        SysIop
    };
    class SyscallDealer {
    public:
        void doSyscall(SyscallOrigin origin, i16 sys);
    private:
        void resetEe(raw_reference<vm::EmuVM> vm);
    };
}
