#include <unordered_map>
#include <functional>
#include <pshook/hk_psx.h>

#include <iop/iop_core.h>
namespace cosmic::pshook {
    struct PsxRegs eeSavedCtx{};
    struct PsxRegs iopSavedCtx{};

    std::mutex EstablishHook::owner{};
    u8 EstablishHook::isEe{1};

    using psxCall = std::function<void()>;
    static std::unordered_map<u32, psxCall> calls{
        {0x1a000014, psxAbs}
    };

    void EstablishHook::hookIoPsx(u32 base, iop::IoMipsCore& psx) {
        u32 ib{};
        switch (base) {
        case 0xa0: ib = 0x1a000000; break;
        case 0xb0: ib = 0x1b000000; break;
        case 0xc0: ib = 0x1c000000; break;
        }
        psxCall handler{};

        if (!ib)
            return;
        std::scoped_lock<std::mutex> lock{owner};
        if (isEe) {
            std::memcpy(&eeSavedCtx, &r, sizeof(r));
            isEe = 0;
        }
        r.a0 = BitCast<PsxR*>(&psx.ioGPRs[4]);
        r.v0 = BitCast<PsxR*>(&psx.ioGPRs[2]);
        r.ra = BitCast<PsxR*>(&psx.ioGPRs[31]);
        r.pc = BitCast<PsxR*>(&psx.ioPc);

        if (ib == 0x1a000000)
            if (psx.ioGPRs[9] != 0x28 && psx.ioGPRs[9] != 0xe)
                if (calls.contains(ib | psx.ioGPRs[9]))
                    handler = calls[ib | psx.ioGPRs[9]];
        if (handler)
            std::invoke(handler);
    }
}