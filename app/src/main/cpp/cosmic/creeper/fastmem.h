#pragma once

#include <common/types.h>
#include <mio/mem_pipe.h>

#include <map>
namespace cosmic::creeper {
    class CachedFastPc {
    public:
        CachedFastPc();
        bool isFastMemoryEnb{true};

        bool checkPc(u32 pc);
        void pushVpc(u32 pc, u8* vpc);
        std::pair<u32, bool> fastFetch(u32 pc);
    private:
        std::map<u32, u8*> savedPc;
    };
}