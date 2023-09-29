#pragma once

#include <cassert>
#include <functional>

namespace zenith {
    namespace exception {
        class runtime_fault : public std::runtime_error {
        public:
            runtime_fault(const std::string& faultMessage) : std::runtime_error(faultMessage) {}
            runtime_fault(const char* faultMessage) : std::runtime_error(faultMessage) {}
        };
    }
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
