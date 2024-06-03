package emu.cosmic.data
import android.content.Context
import android.content.res.Resources.NotFoundException
import emu.cosmic.CosmicApplication

class CosmicSettings private constructor(context: Context) {
    var appStorage by DelegateDataStore<String>(SettingContainer(context, SettingsKeys.AppStorage))

    var gpuTurboMode by DelegateDataStore<Boolean>(SettingContainer(context, SettingsKeys.GpuTurboMode))

    var customDriver by DelegateDataStore<String>(SettingContainer(context, SettingsKeys.CustomDriver))

    var eeMode by DelegateDataStore<Int>(SettingContainer(context, SettingsKeys.EEMode))

    var biosPath by DelegateDataStore<String>(SettingContainer(context, SettingsKeys.BiosPath))

    // Creating a static object to store all our configurations
    // This object will reside in the global heap memory (Accessible to JNI)
    companion object {
        val globalSettings by lazy { CosmicSettings(CosmicApplication.context) }
        var updateSettings: Boolean = false

        private val dsCachedSet = LinkedHashMap<String, Any>()
        @JvmStatic
        fun getDataStoreValue(config: String) : Any {
            dsCachedSet.clear()

            dsCachedSet["dsdb_app_storage"] = globalSettings.appStorage
            dsCachedSet["dsdb_gpu_turbo_mode"] = globalSettings.gpuTurboMode
            dsCachedSet["dsdb_gpu_custom_driver"] = globalSettings.customDriver
            dsCachedSet["dsdb_ee_mode"] = globalSettings.eeMode
            dsCachedSet["dsdb_bios_path"] = globalSettings.biosPath

            if (!dsCachedSet.containsValue(config)) {
                throw NotFoundException(config)
            }
            return dsCachedSet.getValue(config)
        }
    }
}