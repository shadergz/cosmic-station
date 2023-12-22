#include <creeper/fastmem.h>

namespace cosmic::creeper {
    CachedFastPc::CachedFastPc() {
        savedPc.clear();
    }
    void CachedFastPc::pushVpc(u32 pc, u8* vpc) {
        if (checkPc(pc))
            return;
        else
            savedPc[pc & 0xfffff000] = vpc;
    }
    bool CachedFastPc::checkPc(u32 pc) {
        return savedPc.contains(pc & 0xfffff000);
    }
    std::pair<u32, bool> CachedFastPc::fastFetch(u32 pc) {
        if (!checkPc(pc))
            return std::make_pair(0, false);
        auto roMem{savedPc[pc & 0xfffff000]};
        auto pcVal{reinterpret_cast<u32*>(roMem + (pc & 0xfff))};
        return std::make_pair(*pcVal, true);
    }
}