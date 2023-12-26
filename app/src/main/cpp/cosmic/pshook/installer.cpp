#include <unordered_map>
#include <functional>
#include <pshook/installer.h>

#include <iop/iop_core.h>
namespace cosmic::pshook {
    struct PsxRegs eeSavedCtx{};
    struct PsxRegs iopSavedCtx{};

    std::mutex HookOwner::owner{};
    u8 HookOwner::isEe{1};

    using psxCall = std::function<void()>;
    static std::unordered_map<u32, psxCall> calls{
        {0x1a000014, psxAbs}
    };

    void HookOwner::hookIoPsx(u32 base, iop::IoMipsCore& psx) {
        u32 ib{};
        switch (base) {
        case 0xa0: ib = 0x1a000000; break;
        case 0xb0: ib = 0x1b000000; break;
        case 0xc0: ib = 0x1c000000; break;
        }
        psxCall handler{};

        if (!ib)
            return;
        owner.lock();
        if (isEe) {
            std::memcpy(&eeSavedCtx, &r, sizeof(r));
            isEe = 0;
        }
        r.a0 = &psx.ioGPRs[4];
        r.v0 = &psx.ioGPRs[2];
        r.ra = &psx.ioGPRs[31];
        r.pc = &psx.ioPc;

        if (ib == 0x1a000000)
            if (psx.ioGPRs[9] != 0x28 && psx.ioGPRs[9] != 0xe)
                if (calls.contains(ib | psx.ioGPRs[9]))
                    handler = calls[ib | psx.ioGPRs[9]];
        if (handler)
            std::invoke(handler);
        owner.unlock();
    }
}