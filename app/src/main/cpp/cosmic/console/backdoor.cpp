#include <console/backdoor.h>

namespace cosmic {
    std::shared_ptr<console::BackDoor> outside;
}
namespace cosmic::console {
    BackDoor::BackDoor(vm::EmuVm& aliveVm) {
        vm = std::make_unique<Ref<vm::EmuVm>>(std::ref(aliveVm));
        echo.lock();
        vmRefs = 1;
    }
    Ref<vm::EmuVm> BackDoor::openVm() {
        std::thread::id nub{};
        if (owner == nub && owner != std::this_thread::get_id()) {
            while (echo.try_lock()) {
                std::this_thread::sleep_for(
                    std::chrono::nanoseconds(2'245));
                std::this_thread::yield();
            }
            owner = std::this_thread::get_id();

        }
        if (owner != std::this_thread::get_id())
            throw AppErr("This resource should have the lock held until the object is released");
        Ref<vm::EmuVm> vmRef{};
        if (vmRefs) {
            vmRef = *vm;
            vmRefs++;
        }
        return vmRef;
    }
    void BackDoor::leaveVm(Ref<vm::EmuVm> lvm) {
        if (echo.try_lock()) {
            if (owner != std::this_thread::get_id())
                throw AppErr("The program flow is broken, review the usage of BackDoor in the code");
        }
        vmRefs--;
        if (!vm || vmRefs <= 0) {
            vm.reset();
            vm = std::make_unique<Ref<vm::EmuVm>>(lvm);
            vmRefs = 1;
        }
        owner = {};
        echo.unlock();
    }
}
