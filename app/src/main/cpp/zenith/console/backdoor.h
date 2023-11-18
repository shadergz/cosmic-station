#pragma once

#include <common/types.h>
#include <thread>
#include <mutex>

namespace zenith::console {
    class EmuVM;

    class RedPillow {
    public:
        RedPillow(EmuVM& aliveVm);

        raw_reference<EmuVM> openVm();
        void leaveVm(raw_reference<EmuVM> lvm);

    private:
        std::thread::id owner;
        std::unique_lock<std::mutex> mutual;
        std::unique_ptr<raw_reference<EmuVM>> vm;
        i32 vmRefs;
    };
}

namespace zenith {
    extern std::shared_ptr<console::RedPillow> redBox;
}

