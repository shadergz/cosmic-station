#include <range/v3/algorithm.hpp>

#include <hle/group_mgr.h>
namespace zenith::hle {
    bool HLEBiosGroup::isAlreadyAdded(i32 is[2], bool usePos) {
        bool alreadyAdded{};
        for (const auto& bios : biosList) {
            if (alreadyAdded)
                break;
            alreadyAdded = bios.isSame(is, usePos);
        }
        return alreadyAdded;
    }

    bool HLEBiosGroup::rmFromStorage(i32 rmBy[2], bool usePos) {
        bool hasRemoved{};
        biosList.remove_if([rmBy, usePos, &hasRemoved](const auto& bios) {
            hasRemoved = bios.isSame(rmBy, usePos);
            return hasRemoved;
        });
        return hasRemoved;
    }

    void HLEBiosGroup::discardAll() {
        if (systemBios)
            systemBios.reset();
        biosList.clear();
    }

    i32 HLEBiosGroup::choice(i32 chBy[2], bool usePos) {
        i32 previous{};
        if (systemBios) {
            previous = systemBios->position;
            systemBios.reset();
        }

        // All non-selected kernels will have their `selected` flag cleared
        auto picked{ranges::find_if(biosList, [chBy, usePos](auto& bios) {
            auto is{bios.isSame(chBy, usePos)};
            bios.selected = is;
            return is;
        })};
        if (picked == biosList.end())
            return -1;

        systemBios = std::make_unique<BiosInfo>(*picked);
        return previous;
    }

    bool HLEBiosGroup::loadFrom(jobject model, i32 ldBy[2], bool usePos) {
        bool loaded{};
        auto biosSelected{ranges::find_if(biosList, [ldBy, usePos](const auto& bios) {
            return bios.isSame(ldBy, usePos);
        })};

        if (biosSelected != biosList.end()) {
            biosSelected->fillInstance(model);
            loaded = true;
        }
        return loaded;
    }

    bool HLEBiosGroup::storeAndFill(jobject model, BiosInfo&& bios) {
        if (!isCrucial && bios.selected)
            isCrucial = true;
        if (!loader.loadBios(android, bios))
            return false;

        bios.fillInstance(model);
        biosList.push_back(std::move(bios));
        return true;
    }
}
