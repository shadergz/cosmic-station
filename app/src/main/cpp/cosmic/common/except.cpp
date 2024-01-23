#include <common/except.h>
#include <common/global.h>

namespace cosmic {
    jclass exceptionActivity{};
    CosmicException::CosmicException(const std::string& format) :
        std::runtime_error(format) {
        user->error("An exception of type CosmicException was raised due to: {}", format);

        msg = cosmicEnv->NewStringUTF(format.c_str());
        title = lookupByActivity();
        alertUser();
    }
    jstring CosmicException::lookupByActivity() {
        const jclass emulation{cosmicEnv->FindClass("emu/cosmic/EmulationActivity")};
        if (cosmicEnv->IsSameObject(exceptionActivity, emulation))  {
            return cosmicEnv->NewStringUTF("Emulation Scene");
        }
        return cosmicEnv->NewStringUTF("General Exception");
    }

    void CosmicException::alertUser() {
        alert = cosmicEnv->GetStaticMethodID(exceptionActivity,
            "displayAlert", "(Ljava/lang/String;Ljava/lang/String;)V");
        if (alert) {
            cosmicEnv->CallStaticVoidMethod(exceptionActivity, alert, title, msg);
        }
        cosmicEnv->DeleteLocalRef(title);
        cosmicEnv->DeleteLocalRef(msg);
    }

    void CosmicException::setExceptionClass(jobject super) {
        const jclass emuClass{cosmicEnv->FindClass("emu/cosmic/EmulationActivity")};
        if (cosmicEnv->IsInstanceOf(super, emuClass)) {
            exceptionActivity = cosmicEnv->FindClass("emu/cosmic/EmulationActivity");
        }
    }
}