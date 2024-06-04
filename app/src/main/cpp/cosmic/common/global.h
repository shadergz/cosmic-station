#pragma once

#include <common/app.h>
#include <common/logger.h>
#include <os/env.h>
#include <os/system_state.h>
namespace cosmic {
    extern std::unique_ptr<os::OsMachState> states;
    extern std::shared_ptr<GlobalLogger> user;
    extern std::shared_ptr<CoreApplication> app;

    extern thread_local os::CosmicEnv cosmicEnv;
}
