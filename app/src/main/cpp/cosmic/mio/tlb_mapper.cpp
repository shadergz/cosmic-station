// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <mio/mmu_tlb.h>
namespace cosmic::mio {
    void TlbCache::mapTlb(TlbPageEntry& entry) {
        u32 virtNumber{entry.vpn2 * 2 >> entry.pageShift};
        u32 virtPhyInfo[2][3];

        for (u8 evenFlow{}; evenFlow < 2; evenFlow++) {
            // Calculating the virtual page number (VPN) from an entry
            virtPhyInfo[evenFlow][0] = (virtNumber * entry.pageSize) / 4096;
            // Virtual Address
            virtPhyInfo[evenFlow][1] = virtPhyInfo[evenFlow][0] * 4096;
            // Physical Address
            virtPhyInfo[evenFlow][2] = (entry.pfn[0] >> entry.pageShift) * entry.pageSize;
        }

        u32 mapFromPage,
            mapFromAddr,
            physicalAddr;
        u8 odd{1};
        if (entry.valid[0]) {
            odd = {};
        } else if (!entry.valid[1]) {
            throw MmuFail("Virtual page {} does not have any valid information; this is a logical error", virtNumber);
        }
        mapFromAddr = virtPhyInfo[odd][0];
        mapFromPage = virtPhyInfo[odd][1];
        physicalAddr = virtPhyInfo[odd][2];

        u32 mapIndex{(entry.isSPad ? 1024 * 16 : entry.pageSize) / 4096};
        for (u32 phyInd{}; phyInd < mapIndex; phyInd++) {
            // We need to use a physical address to map into
            u8* phyPtr{choiceMemSrc(physicalAddr + phyInd)};

            kernelVtlb[mapFromPage + phyInd] = phyPtr;
            if (mapFromAddr < 0x80000000) {
                supervisorVtlb[mapFromPage + phyInd] = phyPtr;
                userVtlb[mapFromPage + phyInd] = phyPtr;
            } else if (mapFromAddr >= 0xc0000000 && mapFromAddr < 0xe0000000) {
                supervisorVtlb[mapFromPage + phyInd] = phyPtr;
            }
            tlbInfo[mapFromPage + phyInd].cacheMode = entry.cacheMode[1];
        }
    }
    void TlbCache::unmapTlb(TlbPageEntry& entry) {
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
            kernelVtlb[invAroundHere + ini] = 0;
            supervisorVtlb[invAroundHere + ini] = 0;
            userVtlb[invAroundHere + ini] = 0;
        }
    }
}
