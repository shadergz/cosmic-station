#pragma once

#include <memory>

#include <console/virtual_devices.h>
#include <console/global_memory.h>

#include <paper_log.h>

namespace zenith {
    class CoreApplication {
    public:
        CoreApplication();
    private:
        std::shared_ptr<console::GlobalMemory> m_virtualBlocks;
        std::shared_ptr<console::VirtualDevices> m_simulatedDevices;

    };
    extern std::unique_ptr<CoreApplication> zenithApp;
}
