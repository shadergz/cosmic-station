package emu.zenith.data
import android.content.Context
import emu.zenith.ZenithApplication

class ZenithSettings private constructor(context: Context) {
    var rootDirectory by DelegateDataStore<String>(SettingContainer(context, SettingsKeys.RootDirectory))

    companion object {
        // Creating a static object to store all our configurations; this object will reside in the
        // global heap memory (Accessible to JNI)
        val globalSettings by lazy { ZenithSettings(ZenithApplication.context) }
    }
}
