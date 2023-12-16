#include <hle/syscall_gate.h>
#include <console/vm/emu_vm.h>
#include <common/global.h>
namespace cosmic::hle {
    // https://github.com/PCSX2/pcsx2/blob/8c94efd61a437263dc23853c7658053be3c8ba7d/pcsx2/R5900OpcodeImpl.cpp#L99C1-L99C7
    static const std::array<const char*, 256> mipsCustomCallsIds{
        "RFU000_FullReset", "ResetEE", "SetGsCrt", "RFU003", "Exit", "RFU005", "LoadExecPS2",
        "ExecPS2", "RFU008", "RFU009", "AddSbusIntcHandler", "RemoveSbusIntcHandler",
        "Interrupt2Iop", "SetVTLBRefillHandler", "SetVCommonHandler", "SetVInterruptHandler",
        "AddIntcHandler", "RemoveIntcHandler", "AddDmacHandler", "RemoveDmacHandler",
        "_EnableIntc", "_DisableIntc", "_EnableDmac", "_DisableDmac", "_SetAlarm", "_ReleaseAlarm",
        "_iEnableIntc", "_iDisableIntc", "_iEnableDmac", "_iDisableDmac", "_iSetAlarm",
        "_iReleaseAlarm", "CreateThread", "DeleteThread", "StartThread", "ExitThread",
        "ExitDeleteThread", "TerminateThread", "iTerminateThread", "DisableDispatchThread",
        "EnableDispatchThread", "ChangeThreadPriority", "iChangeThreadPriority",
        "RotateThreadReadyQueue", "iRotateThreadReadyQueue", "ReleaseWaitThread",
        "iReleaseWaitThread", "GetThreadId", "ReferThreadStatus", "iReferThreadStatus",
        "SleepThread", "WakeupThread", "_iWakeupThread", "CancelWakeupThread",
        "iCancelWakeupThread", "SuspendThread", "iSuspendThread", "ResumeThread", "iResumeThread",
        "JoinThread", "RFU060", "RFU061", "EndOfHeap", "RFU063", "CreateSema", "DeleteSema",
        "SignalSema", "iSignalSema", "WaitSema", "PollSema", "iPollSema", "ReferSemaStatus",
        "iReferSemaStatus", "RFU073", "SetOsdConfigParam", "GetOsdConfigParam", "GetGsHParam",
        "GetGsVParam", "SetGsHParam", "SetGsVParam",
        "RFU080_CreateEventFlag", "RFU081_DeleteEventFlag",
        "RFU082_SetEventFlag", "RFU083_iSetEventFlag",
        "RFU084_ClearEventFlag", "RFU085_iClearEventFlag",
        "RFU086_WaitEventFlag", "RFU087_PollEventFlag",
        "RFU088_iPollEventFlag", "RFU089_ReferEventFlagStatus",
        "RFU090_iReferEventFlagStatus", "RFU091_GetEntryAddress",
        "EnableIntcHandler_iEnableIntcHandler",
        "DisableIntcHandler_iDisableIntcHandler",
        "EnableDmacHandler_iEnableDmacHandler",
        "DisableDmacHandler_iDisableDmacHandler",
        "KSeg0", "EnableCache", "DisableCache", "GetCop0", "FlushCache", "RFU101", "CpuConfig",
        "iGetCop0", "iFlushCache", "RFU105", "iCpuConfig", "sceSifStopDma", "SetCPUTimerHandler",
        "SetCPUTimer", "SetOsdConfigParam2", "GetOsdConfigParam2", "GsGetIMR_iGsGetIMR",
        "GsGetIMR_iGsPutIMR", "SetPgifHandler", "SetVSyncFlag", "RFU116", "print",
        "sceSifDmaStat_isceSifDmaStat", "sceSifSetDma_isceSifSetDma",
        "sceSifSetDChain_isceSifSetDChain", "sceSifSetReg", "sceSifGetReg", "ExecOSD", "Deci2Call",
        "PSMode", "MachineType", "GetMemorySize",
    };
    void SyscallDealer::resetEe(raw_reference<console::vm::EmuVM> vm) {
        i32 resetParam{*vm->mips->gprAt<i32>(Param0)};
        switch (resetParam) {
        case 0:
            vm->sharedPipe->controller->resetMA();
            break;
        case 1:
            vm->vu01->vpu1Dlo.resetVU();
            break;
        case 6:
            vm->mpegDecoder->resetDecoder();
        }
        if (resetParam == 5 || resetParam == 4)
            ;
    }

    void SyscallDealer::doSyscall(SyscallOrigin origin, i16 sys) {
        fmt::memory_buffer sysDev{};
        fmt::format_to(back_inserter(sysDev), "Syscall with the name {} ", mipsCustomCallsIds.at(static_cast<u64>(sys)));

        auto vm{redBox->openVm()};
        if (origin == SysEmotionEngine) {
            fmt::format_to(back_inserter(sysDev), "E.E. over {} ", vm->mips->ctrl0.status.mode == 0 ? "Kernel" : "Super/User");
            bool sysExist{true};
            switch (sys) {
            case 0x01:
                // void ResetEE(i32 resetFlag);
                resetEe(vm); break;
            default: sysExist = false; break;
            }
            if (sysExist)
                fmt::format_to(back_inserter(sysDev), "exists and has been treated");
            else
                fmt::format_to(back_inserter(sysDev), "does not exist");
        }
        redBox->leaveVm(vm);
    }
}
