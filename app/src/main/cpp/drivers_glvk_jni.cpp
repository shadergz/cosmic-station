#include <fcntl.h>
#include <jni.h>

#include <linux/types.h>
#include <linux/ioctl.h>

#include <cosmic/common/types.h>
struct kgslDeviceGetProperty {
    cosmic::u32 type;
    void __user* value;
    __kernel_size_t sizeBytes;
};

static constexpr cosmic::u8 kgslPropPWRCTRL{0xe};
static constexpr auto ioctlKgslSetProperty{_IOW(0x09, 0x32, struct kgslDeviceGetProperty)};
void driverSetTurbo(bool mode) {
    cosmic::u32 enable{mode ? 0U : 1U};
    kgslDeviceGetProperty prop{
        .type = kgslPropPWRCTRL,
        .value = cosmic::bit_cast<void*>(&enable),
        .sizeBytes = sizeof(enable)};

    cosmic::i32 kgslFd{open("/dev/kgsl-3d0", O_RDWR)};
    if (kgslFd < 0)
        return;

    ioctl(kgslFd, ioctlKgslSetProperty, &prop);
    close(kgslFd);
}

extern "C"
JNIEXPORT void JNICALL
Java_emu_cosmic_helpers_DriverHelper_00024Companion_switchTurboMode(JNIEnv* env, jobject thiz, jboolean enable) {
    driverSetTurbo(enable);
}