#pragma once

#include <sys/stat.h>
#include <unistd.h>
#include <exception>

#include <paper_assert.h>

namespace zenith {

    struct ZenFile {
        static constexpr auto invalidFileDescriptor{-1};
        using FileStat = struct stat;
    public:
        ZenFile() :
            basicFd(-1) {}

        ~ZenFile() {
            if (basicFd != invalidFileDescriptor)
                close(basicFd);
        }
        FileStat lastStates;
        int basicFd;

        void operator=(int fileNativeFd) {
            if (fileNativeFd == invalidFileDescriptor) {
                // throw std::runtime_error("");
            }
            basicFd = fileNativeFd;
            fstat(basicFd, &lastStates);

            PaperRtAssertPersistent(lastStates.st_mode & S_IFMT, S_IFREG, "");
        }
        auto operator*()-> int {
            return basicFd;
        }

    };
}
