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
        Ref<vm::EmuVm> openVm();
        void leaveVm(Ref<vm::EmuVm> lvm);
    private:
        std::thread::id owner;
        std::mutex echo;
        std::unique_ptr<Ref<vm::EmuVm>> vm;
        i32 vmRefs;
    };
}
namespace cosmic {
    extern std::shared_ptr<console::BackDoor> outside;
}

