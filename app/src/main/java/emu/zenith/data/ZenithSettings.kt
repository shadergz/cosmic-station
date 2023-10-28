package emu.zenith.data
import android.content.Context
import android.content.res.Resources.NotFoundException
import emu.zenith.ZenithApplication

class ZenithSettings private constructor(context: Context) {
    var appStorage by DelegateDataStore<String>(SettingContainer(context, SettingsKeys.AppStorage))
    var gpuTurboMode by DelegateDataStore<Boolean>(SettingContainer(context, SettingsKeys.GpuTurboMode))
    var customDriver by DelegateDataStore<String>(SettingContainer(context, SettingsKeys.CustomDriver))
    var eeMode by DelegateDataStore<Int>(SettingContainer(context, SettingsKeys.EEMode))

    // Creating a static object to store all our configurations; this object will reside in the
    // global heap memory (Accessible to JNI)
    companion object {
        val globalSettings by lazy { ZenithSettings(ZenithApplication.context) }

        @JvmStatic
        fun getDataStoreValue(config: String) : Any {
            return when (config) {
                "dsdbAppStorage" -> globalSettings.appStorage
                "dsdbGpuTurboMode" -> globalSettings.gpuTurboMode
                "dsdbGpuCustomDriver" -> globalSettings.customDriver
                "dsdbEeMode" -> globalSettings.eeMode
                else -> {
                    throw NotFoundException("")
                }
            }
        }
    }
}