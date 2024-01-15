#pragma once

#include <java/device_handler.h>
#include <common/app.h>
#include <common/logger.h>
namespace cosmic {
    extern std::unique_ptr<java::JvmManager> device;
    extern std::shared_ptr<GlobalLogger> user;
    extern std::shared_ptr<CoreApplication> app;
}
