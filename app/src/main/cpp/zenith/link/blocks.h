#pragma once

#include <link/main_memory.h>
#include <link/bios_memory.h>

namespace zenith::link {
    class GlobalMemory :
    public LogicalRAMBlock,
        BIOSBlock {};
}
