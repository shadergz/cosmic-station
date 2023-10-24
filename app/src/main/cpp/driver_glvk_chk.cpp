#include <fcntl.h>
#include <jni.h>

#include <linux/types.h>
#include <linux/ioctl.h>

#include <zenith/types.h>
struct kgslDeviceGetProperty {
    unsigned int type;
    void __user* value;
    __kernel_size_t sizeBytes;
};

static constexpr zenith::u8 kgslPropPWRCTRL{0xe};
static constexpr auto ioctlKGSLSetProperty{_IOW(0x09, 0x32, struct kgslDeviceGetProperty)};
void driverSetTurbo(bool mode) {
    zenith::u32 enable{mode ? 0U : 1U};
    kgslDeviceGetProperty prop{
        .type = kgslPropPWRCTRL,
        .value = zenith::bit_cast<void*>(&enable),
        .sizeBytes = sizeof(enable)};

    zenith::i32 kgslFd{open("/dev/kgsl-3d0", O_RDWR)};
    if (kgslFd < 0)
        return;

    ioctl(kgslFd, ioctlKGSLSetProperty, &prop);
    close(kgslFd);
}

extern "C"
JNIEXPORT void JNICALL
Java_emu_zenith_helpers_GpuHelper_00024Companion_switchTurboMode(JNIEnv *env, jobject thiz, jboolean enable) {
    driverSetTurbo(enable);
}