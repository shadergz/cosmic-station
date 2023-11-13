#include <mio/mmu_tlb.h>

namespace zenith::mio {
    void TLBCache::mapTLB(TLBPageEntry& entry) {
        u32 virtNumber{entry.vpn2 * 2 >> entry.pageShift};
        u32 virtPhyInfo[2][3];

        for (u8 evenFlow{}; evenFlow < 2; evenFlow++) {
            // Calculating the virtual page number (VPN) from an entry
            virtPhyInfo[evenFlow][0] = (virtNumber * entry.pageSize) / 4096;
            // Virtual Address
            virtPhyInfo[evenFlow][1] = virtPhyInfo[evenFlow][0] * 4096;
            // Physical Address
            virtPhyInfo[evenFlow][2] = (entry.pfn[0] >>entry.pageShift) * entry.pageSize;
        }

        u32 mapFromPage{}, mapFromAddr{}, physicalAddr{};
        if (entry.valid[0]) {
            mapFromAddr = virtPhyInfo[0][0];
            mapFromPage = virtPhyInfo[0][1];
            physicalAddr = virtPhyInfo[0][2];
        } else if (entry.valid[1]) {
            mapFromAddr = virtPhyInfo[1][0];
            mapFromPage = virtPhyInfo[1][1];
            physicalAddr = virtPhyInfo[1][2];
        }

        u32 mapIndex{(entry.isSPad ? 1024 * 16 : entry.pageSize) / 4096};
        for (u32 phyInd{}; phyInd < mapIndex; phyInd++) {
            // We need to use a physical address to map into
            u8* phyPtr{choiceMemSrc(physicalAddr + phyInd)};

            kernelVTLB[mapFromPage + mapIndex] = phyPtr;
            if (mapFromAddr < 0x80000000) {
                supervisorVTLB[mapFromPage + mapIndex] = phyPtr;
                userVTLB[mapFromPage + mapIndex] = phyPtr;
            } else if (mapFromAddr >= 0xc0000000 && mapFromAddr < 0xe0000000) {
                supervisorVTLB[mapFromPage + mapIndex] = phyPtr;
            }
            tlbInfo[mapFromPage + mapIndex].cacheMode = entry.cacheMode[1];
        }
    }
    void TLBCache::unmapTLB(TLBPageEntry& entry) {
        u32 realVPN{(entry.vpn2 * 2) >> entry.pageShift};
        u32 invAroundHere{};
        if (entry.valid[0]) {
            u32 evenPage{realVPN * entry.pageSize / 4096};
            invAroundHere = evenPage;
        } else if (entry.valid[1]) {
            invAroundHere = (realVPN + 1) * entry.pageSize / 4096;
        }
        u32 pageRange{entry.isSPad ? 1024 * 16 : entry.pageSize};
        u32 pageIndex{pageRange / 4096};

        for (u32 ini{}; ini < pageIndex; ini++) {
            kernelVTLB[invAroundHere + ini] = 0;
            supervisorVTLB[invAroundHere + ini] = 0;
            userVTLB[invAroundHere + ini] = 0;
        }
    }
}
