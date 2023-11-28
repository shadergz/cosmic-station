#pragma once

#include <common/types.h>
#include <thread>
#include <mutex>

namespace cosmic::console::vm {
    class EmuVM;
}
namespace cosmic::console {
    class RedPillow {
    public:
        RedPillow(vm::EmuVM& aliveVm);
        raw_reference<vm::EmuVM> openVm();
        void leaveVm(raw_reference<vm::EmuVM> lvm);
    private:
        std::thread::id owner;
        std::unique_lock<std::mutex> mutual;
        std::unique_ptr<raw_reference<vm::EmuVM>> vm;
        i32 vmRefs;
    };
}
namespace cosmic {
    extern std::shared_ptr<console::RedPillow> redBox;
}

