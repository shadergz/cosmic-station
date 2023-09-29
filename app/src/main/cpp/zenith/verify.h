#pragma once

#include <cassert>
#include <functional>

namespace zenith {
    void verifyRtCheck(bool condition, std::function<void()> func);
}

#if defined(DEBUG)
#define VerifyRtAssert(cond)\
    paperRtCheck(cond, [](){\
        assert(cond);\
    })
#else
#define VerifyRtAssert(cond)\
    (void)(cond)
#endif
