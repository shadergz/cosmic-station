#pragma once

#include <java/device_handler.h>
#include <logger.h>

namespace zenith {
    extern std::unique_ptr<java::JvmManager> deviceRes;
    extern std::shared_ptr<GlobalLogger> userLog;
}
