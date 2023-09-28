#pragma once

#include <sys/stat.h>
#include <unistd.h>
#include <paper_debug.h>
namespace zenith {
    using u8 = uint8_t;
    using u16 = uint16_t;

    using i32 = int32_t;
    using u32 = uint32_t;

    using u64 = uint64_t;

    struct ZenFile {
        static constexpr auto invalidFileDescriptor{-1};
        using FileStat = struct stat;
    public:
        ZenFile()
            : basicFd(-1) {}

        ~ZenFile() {
            if (basicFd != invalidFileDescriptor)
                close(basicFd);
        }
        FileStat lastStates;
        int basicFd;

        void operator=(int fileNativeFd) {
            PaperRtAssertPersistent(fileNativeFd == invalidFileDescriptor, "");
            basicFd = fileNativeFd;
            fstat(basicFd, &lastStates);
            PaperRtAssert((lastStates.st_mode & S_IFMT) == S_IFREG, "");
        }
        auto operator*()-> int {
            return basicFd;
        }
    };
}
