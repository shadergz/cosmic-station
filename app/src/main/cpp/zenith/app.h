#pragma once

#include <memory>

#include <console/virtual_devices.h>
#include <java/device_res.h>
#include <paper_log.h>

namespace zenith {
    class CoreApplication {
    public:
        CoreApplication();
    private:
        std::shared_ptr<console::GlobalMemory> m_virtualBlocks;
        std::shared_ptr<console::VirtualDevices> m_simulatedDevices;
    };

    extern std::unique_ptr<java::JvmManager> deviceRes;
    extern std::unique_ptr<CoreApplication> zenithApp;

    extern std::shared_ptr<PalePaper> userLog;
}
