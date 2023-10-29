#pragma once

#include <span>

#include <common/types.h>
namespace zenith::cpu {
    u32 check32(std::span<u8> chkData);
}
