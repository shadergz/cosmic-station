#pragma once

#include <common/types.h>
#include <java/jclasses.h>

namespace cosmic::hle {
    class BiosInfo : java::JavaClass {
    public:
        BiosInfo(JNIEnv* env)
            : java::JavaClass(env, "emu/cosmic/data/BiosInfo") {}
        i32 position;
        DescriptorRaii fd;
        u32 dataCRC;
        bool selected{false};

        java::JNIString dspName;
        java::JNIString details;

        jobject createInstance() override;
        void fillInstance(jobject kotlin) override;
        void chkAndLoad(i32 descriptor);

        bool isSame(i32 is[2], bool usePos = false) const {
            bool equal;
            if (usePos) {
                equal = position == is[1];
            } else {
                std::array<DescriptorRaii::FileStat, 2> stat{};

                fstat(static_cast<i32>(is[0]), &stat[0]);
                fstat(fd.getFd(), &stat[1]);

                equal = stat[0].st_ino == stat[1].st_ino;
            }
            return equal;
        }
    };
}
