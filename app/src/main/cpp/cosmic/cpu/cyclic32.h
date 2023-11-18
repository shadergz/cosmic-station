#pragma once

#include <span>

#include <common/types.h>
namespace cosmic::cpu {
    u32 check32(std::span<u8> chkData);
}
