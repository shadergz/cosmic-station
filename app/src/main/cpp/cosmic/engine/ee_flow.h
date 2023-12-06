#pragma once
#include <common/types.h>
namespace cosmic::engine {
    class EePc {
    public:
        EePc() = default;
        EePc(u32 pc) : pcValue(pc) {}
        auto operator++(i32 lastPC) {
            return pcValue += 4;
        }
        auto operator--(i32 lastPC) {
            return pcValue -= 4;
        }
        auto operator*() {
            return pcValue;
        }
        u32 pcValue{};
    };
    struct EeFlowCtrl {
        i64 cyclesToWaste;
        i64 cycles;
    };
}
