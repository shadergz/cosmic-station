#include <console/backdoor.h>

namespace cosmic {
    std::shared_ptr<console::RedPillow> redBox;
}
namespace cosmic::console {
    RedPillow::RedPillow(EmuVM& aliveVm) {
        vm = std::make_unique<raw_reference<EmuVM>>(std::ref(aliveVm));
        mutual = std::unique_lock<std::mutex>();
        vmRefs = 1;
    }
    raw_reference<EmuVM> RedPillow::openVm() {
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
        raw_reference<EmuVM> vmRef{};
        if (vmRefs) {
            vmRef = *vm;
            vmRefs++;
        }
        return vmRef;
    }
    void RedPillow::leaveVm(raw_reference<EmuVM> lvm) {
        if (!mutual.try_lock()) {
            if (owner != std::this_thread::get_id())
                throw AppFail("The program flow is broken, review the usage of RedPillow in the code");
        }
        vmRefs--;
        if (!vm || vmRefs <= 0) {
            vm.reset();
            vm = std::make_unique<raw_reference<EmuVM>>(lvm);
            vmRefs = 1;
        }
        owner = {};
        mutual.unlock();
    }
}
