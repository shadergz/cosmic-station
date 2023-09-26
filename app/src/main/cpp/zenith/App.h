#pragma once

#include <memory>

#include <console/VirtualDevices.h>
#include <java/DeviceRes.h>
#include <PaperLog.h>

namespace zenith {
    class CoreApplication {
    public:
        CoreApplication();
    private:
        std::shared_ptr<console::GlobalMemory> virtualBlocks;
        std::shared_ptr<console::VirtualDevices> simulatedDevices;
    };

    extern std::unique_ptr<java::JvmManager> deviceRes;
    extern std::unique_ptr<CoreApplication> zenithApp;

    extern std::shared_ptr<PalePaper> userLog;
}
