#include <kernel/group.h>

#include <range/v3/algorithm.hpp>

namespace zenith::kernel {

    bool KernelsGroup::isAlreadyAdded(i32 is[2], bool usePos) {
        bool alreadyAdded{};
        for (const auto& kernel : kernels) {
            if (alreadyAdded)
                break;
            alreadyAdded = kernel.isSame(is, usePos);
        }
        return alreadyAdded;
    }

    bool KernelsGroup::rmFromStorage(i32 rmBy[2], bool usePos) {
        bool hasRemoved{};
        kernels.remove_if([rmBy, usePos, &hasRemoved](const auto& kernel) {
            hasRemoved = kernel.isSame(rmBy, usePos);
            return hasRemoved;
        });
        return hasRemoved;
    }

    i32 KernelsGroup::choice(i32 chBy[2], bool usePos) {
        i32 previous{};

        if (systemBios) {
            previous = systemBios->position;
            systemBios->selected = false;
            kernels.push_front(std::move(*systemBios.release()));
        }
        auto picked{ranges::find_if(kernels, [chBy, usePos](const auto& kernel) {
            // All non-selected kernels will have their `selected` flag cleared
            return kernel.isSame(chBy, usePos);
        })};

        if (picked == kernels.end())
            return false;

        picked->selected = true;
        systemBios = std::make_unique<KernelModel>(std::move(*picked));
        kernels.erase(picked);

        return previous;
    }

    bool KernelsGroup::loadFrom(jobject model, i32 ldBy[2], bool usePos) {
        bool loaded{};
        auto kernel{ranges::find_if(kernels, [ldBy, usePos](const auto& kernel) {
            return kernel.isSame(ldBy, usePos);
        })};

        if (kernel != kernels.end()) {
            kernel->fillInstance(model);
            loaded = true;
        }
        return loaded;
    }

    bool KernelsGroup::storeAndFill(jobject model, KernelModel&& kernel) {
        if (!isCrucial && kernel.selected)
            isCrucial = true;
        if (!loader.loadBios(android, kernel))
            return false;

        kernel.fillInstance(model);
        kernels.push_front(std::move(kernel));
        return true;
    }
}
