// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <range/v3/algorithm.hpp>
#include <sys/system_properties.h>

#include <cassert>
#include <common/global.h>
#include <common/except.h>

namespace cosmic {
    std::unique_ptr<java::JvmManager> device;
    std::shared_ptr<GlobalLogger> userLog;
    std::shared_ptr<CoreApplication> app;

    CoreApplication::CoreApplication()
        : simulated(std::make_shared<console::VirtDevices>()) {
        apiLevel = android_get_device_api_level();

        std::array<bool, 1> feats{
            riscFeatures.haveCrc32C()
        };
        auto failed = ranges::find_if(feats, [](auto test) { return !test; });
        if (failed != feats.end()) {
            throw AppFail("Some of the required ARM ISA sets aren't available on your host processor");
        }

        userLog->success("Device {} accepted as the host device, Android API {}", getDeviceName(), apiLevel);

        scene = std::make_shared<gpu::ExhibitionEngine>();
        vm = std::make_unique<console::EmuVM>(device->android, simulated, scene);
        vm->resetVM();
    }

    std::shared_ptr<hle::HLEBiosGroup> CoreApplication::getBiosMgr() {
        auto group{vm->biosHLE->group};
        return group;
    }

    const std::string& CoreApplication::getDeviceName() {
        if (artDeviceName.empty()) {
            std::array<char, 40> model;
            __system_property_get("ro.product.model", model.data());
            artDeviceName = std::string(model.data());
        }
        return artDeviceName;
    }
}
