#include <fcntl.h>
#include <range/v3/algorithm.hpp>

#include <common/global.h>
#include <hle/group_mgr.h>

namespace cosmic::hle {
    HleBiosGroup::HleBiosGroup() {}
    void HleBiosGroup::readBios(std::span<u8> loadHere) {
        const auto biosPath{*(device->getStates()->biosPath)};
        BiosInfo info{};
        if (slotBios) {
            slotBios.reset();
        }

        info.fd = DescriptorRaii(open(biosPath.c_str(), O_RDONLY), true);
        slotBios = std::make_unique<BiosInfo>(std::move(info));
        if (!slotBios) {
            throw NonAbort("Wait, there is no BIOS available in the slot");
        }
        loader.triggerBios(*slotBios);
        loader.placeBios(loadHere);
    }

    bool HleBiosGroup::isAlreadyAdded(i32 is[2], bool usePos) {
        bool alreadyAdded{};
        if (slotBios && slotBios->isSame(is, usePos))
            return true;

        for (const auto& bios : biosList) {
            if (alreadyAdded)
                break;
            alreadyAdded = bios.isSame(is, usePos);
        }
        return alreadyAdded;
    }
    bool HleBiosGroup::rmFromStorage(i32 rmBy[2], bool usePos) {
        bool hasRemoved{};
        biosList.remove_if([rmBy, usePos, &hasRemoved](const auto& bios) {
            hasRemoved = bios.isSame(rmBy, usePos);
            return hasRemoved;
        });
        return hasRemoved;
    }
    void HleBiosGroup::discardAll() {
        if (slotBios)
            slotBios.reset();
        biosList.clear();
    }

    i32 HleBiosGroup::choice(i32 chBy[2], bool usePos) {
        i32 previous{};
        if (slotBios) {
            previous = slotBios->position;
            biosList.push_back(std::move(*slotBios));
            slotBios.reset();
        }

        // All non-selected kernels will have their `selected` flag cleared
        auto picked{ranges::find_if(biosList, [chBy, usePos](auto& bios) {
            auto is{bios.isSame(chBy, usePos)};
            bios.selected = is;
            return is;
        })};
        if (picked == biosList.end())
            return -1;

        slotBios = std::make_unique<BiosInfo>(std::move(*picked));
        biosList.erase(picked);
        return previous;
    }

    bool HleBiosGroup::loadBiosBy(jobject model, i32 ldBy[2], bool usePos) {
        bool loaded{};
        if (slotBios && slotBios->isSame(ldBy, usePos)) {
            slotBios->fillInstance(model);
            return true;
        }

        auto biosSelected{ranges::find_if(biosList, [ldBy, usePos](const auto& bios) {
            return bios.isSame(ldBy, usePos);
        })};

        if (biosSelected != biosList.end()) {
            biosSelected->fillInstance(model);
            loaded = true;
        }
        return loaded;
    }
    bool HleBiosGroup::storeAndFill(jobject model, BiosInfo&& bios) {
        if (!isCrucial && bios.selected)
            isCrucial = true;
        if (!loader.fetchBiosInfo(bios))
            return false;

        bios.fillInstance(model);
        if (!slotBios) {
            slotBios = std::make_unique<BiosInfo>(std::move(bios));
        } else {
            biosList.emplace_back(std::move(bios));
        }

        return true;
    }
}
