#include <console/backdoor.h>

namespace cosmic {
    std::shared_ptr<console::BackDoor> redBox;
}
namespace cosmic::console {
    BackDoor::BackDoor(vm::EmuVM& aliveVm) {
        vm = std::make_unique<raw_reference<vm::EmuVM>>(std::ref(aliveVm));
        echo.lock();
        vmRefs = 1;
    }
    raw_reference<vm::EmuVM> BackDoor::openVm() {
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
            throw AppFail("This resource should have the lock held until the object is released");
        raw_reference<vm::EmuVM> vmRef{};
        if (vmRefs) {
            vmRef = *vm;
            vmRefs++;
        }
        return vmRef;
    }
    void BackDoor::leaveVm(raw_reference<vm::EmuVM> lvm) {
        if (echo.try_lock()) {
            if (owner != std::this_thread::get_id())
                throw AppFail("The program flow is broken, review the usage of BackDoor in the code");
        }
        vmRefs--;
        if (!vm || vmRefs <= 0) {
            vm.reset();
            vm = std::make_unique<raw_reference<vm::EmuVM>>(lvm);
            vmRefs = 1;
        }
        owner = {};
        echo.unlock();
    }
}
