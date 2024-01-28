#pragma once

#include <vector>
#include <common/types.h>
namespace cosmic::gamedb {
    struct SwitchPath {
        u32 gameCase, rCase;
    };

    using SwitchPatches = std::vector<SwitchPath>;
}