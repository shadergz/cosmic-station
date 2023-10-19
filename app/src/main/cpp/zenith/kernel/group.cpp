#include <kernel/group.h>

#include <range/v3/algorithm.hpp>

namespace zenith::kernel {

    bool KernelsGroup::isAlreadyAdded(u32 is[2], bool useCRC) {
        bool alreadyAdded{};
        for (const auto& kernel : kernels) {
            if (alreadyAdded)
                break;
            alreadyAdded = kernel.isSame(is, useCRC);
        }
        return alreadyAdded;
    }

    bool KernelsGroup::rmFromStorage(u32 rmBy[2], bool useCRC) {
        bool hasRemoved{};
        ranges::remove_if(kernels, [rmBy, useCRC, &hasRemoved](const auto& kernel) {
            hasRemoved = kernel.isSame(rmBy, useCRC);
            return hasRemoved;
        });
        return hasRemoved;
    }

    bool KernelsGroup::choice(u32 chBy[2], bool useCRC) {
        bool picked{};
        for (auto& kernel : kernels) {
            picked = kernel.isSame(chBy, useCRC);
            // All non-selected kernels will have their `selected` flag cleared
            kernel.kSelected = picked;
        }
        return picked;
    }

    bool KernelsGroup::loadFrom(jobject model, u32 ldBy[2], bool useCRC) {
        bool loaded{};
        auto kernel{ranges::find_if(kernels, [ldBy, useCRC](const auto& kernel) {
            return kernel.isSame(ldBy, useCRC);
        })};

        if (kernel != kernels.end()) {
            kernel->fillInstance(model);
            loaded = true;
        }
        return loaded;
    }

    bool KernelsGroup::store(KernelModel &&kernel) {
        if (!isCrucial && kernel.kSelected)
            isCrucial = true;
        if (!loader.loadBios(nullptr, kernel))
            return false;

        kernels.push_back(kernel);
        return true;
    }
}