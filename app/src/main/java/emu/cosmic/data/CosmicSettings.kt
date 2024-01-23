package emu.cosmic.data
import android.content.Context
import android.content.res.Resources.NotFoundException
import emu.cosmic.CosmicApplication

class CosmicSettings private constructor(context: Context) {
    var appStorage by DelegateDataStore<String>(
        SettingContainer(context, SettingsKeys.AppStorage))
    var gpuTurboMode by DelegateDataStore<Boolean>(
        SettingContainer(context, SettingsKeys.GpuTurboMode))
    var customDriver by DelegateDataStore<String>(
        SettingContainer(context, SettingsKeys.CustomDriver))
    var eeMode by DelegateDataStore<Int>(
        SettingContainer(context, SettingsKeys.EEMode))
    var biosPath by DelegateDataStore<String>(
        SettingContainer(context, SettingsKeys.BiosPath))

    // Creating a static object to store all our configurations; this object will reside in the
    // global heap memory (Accessible to JNI)
    companion object {
        val globalSettings by lazy { CosmicSettings(CosmicApplication.context) }
        var updateSettings: Boolean = false
        @Suppress("unused") @JvmStatic
        fun getDataStoreValue(config: String) : Any {
            return when (config) {
                "dsdb_app_storage" -> globalSettings.appStorage
                "dsdb_gpu_turbo_mode" -> globalSettings.gpuTurboMode

                "dsdb_gpu_custom_driver" -> globalSettings.customDriver
                "dsdb_ee_mode" -> globalSettings.eeMode

                "dsdb_bios_path" -> globalSettings.biosPath

                else -> {
                    throw NotFoundException(config)
                }
            }
        }
    }
}