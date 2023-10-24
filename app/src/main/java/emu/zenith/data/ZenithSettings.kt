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
        fun getEnvStateVar(config: String) : Any {
            return when (config) {
                "App Storage Directory" -> globalSettings.appStorage
                "GPU Turbo Mode" -> globalSettings.gpuTurboMode
                else -> 0
            }
        }
    }
}