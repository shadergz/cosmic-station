#include <console/backdoor.h>

namespace cosmic {
    std::shared_ptr<console::BackDoor> redBox;
}
namespace cosmic::console {
    BackDoor::BackDoor(vm::EmuVM& aliveVm) {
        vm = std::make_unique<raw_reference<vm::EmuVM>>(std::ref(aliveVm));
        mutual = std::unique_lock<std::mutex>();
        vmRefs = 1;
    }
    raw_reference<vm::EmuVM> BackDoor::openVm() {
        std::thread::id nub{};
        if (owner != std::this_thread::get_id()) {
            mutual.lock();
            owner = nub;
        }
        if (owner != nub) {
            if (owner != std::this_thread::get_id())
                throw AppFail("This resource should have the lock held until the object is released");
        } else {
            owner = std::this_thread::get_id();
        }
        raw_reference<vm::EmuVM> vmRef{};
        if (vmRefs) {
            vmRef = *vm;
            vmRefs++;
        }
        return vmRef;
    }
    void BackDoor::leaveVm(raw_reference<vm::EmuVM> lvm) {
        if (!mutual.try_lock()) {
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
        mutual.unlock();
    }
}
