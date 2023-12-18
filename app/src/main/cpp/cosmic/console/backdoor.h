#pragma once
#include <thread>
#include <mutex>

#include <common/types.h>
namespace cosmic::vm {
    class EmuVM;
}
namespace cosmic::console {
    class BackDoor {
    public:
        BackDoor(vm::EmuVM& aliveVm);
        raw_reference<vm::EmuVM> openVm();
        void leaveVm(raw_reference<vm::EmuVM> lvm);
    private:
        std::thread::id owner;
        std::mutex echo;
        std::unique_ptr<raw_reference<vm::EmuVM>> vm;
        i32 vmRefs;
    };
}
namespace cosmic {
    extern std::shared_ptr<console::BackDoor> redBox;
}

