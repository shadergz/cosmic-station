package emu.zenith.data
import android.content.Context
import emu.zenith.ZenithApplication

class ZenithSettings private constructor(context: Context) {
    var appStorage by DelegateDataStore<String>(
        SettingContainer(context, SettingsKeys.AppStorage))
    var gpuTurboMode by DelegateDataStore<Boolean>(
        SettingContainer(context, SettingsKeys.GpuTurboMode))

    companion object {
        // Creating a static object to store all our configurations; this object will reside in the
        // global heap memory (Accessible to JNI)
        val globalSettings by lazy { ZenithSettings(ZenithApplication.context) }

        @JvmStatic
        fun getDataStoreValue(config: String) : Any {
            return when (config) {
                "DSDB_APP_STORAGE" -> globalSettings.appStorage
                "DSDB_GPU_TURBO_MODE" -> globalSettings.gpuTurboMode
                else -> 0
            }
        }
    }
}