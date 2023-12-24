#pragma once
#include <thread>
#include <mutex>

#include <common/types.h>
namespace cosmic::vm {
    class EmuVm;
}
namespace cosmic::console {
    class BackDoor {
    public:
        BackDoor(vm::EmuVm& aliveVm);
        raw_reference<vm::EmuVm> openVm();
        void leaveVm(raw_reference<vm::EmuVm> lvm);
    private:
        std::thread::id owner;
        std::mutex echo;
        std::unique_ptr<raw_reference<vm::EmuVm>> vm;
        i32 vmRefs;
    };
}
namespace cosmic {
    extern std::shared_ptr<console::BackDoor> redBox;
}

