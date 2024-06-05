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

    // Creating a static object to store all our configurations
    // This object will reside in the global heap memory (Accessible to JNI)
    companion object {
        val globalSettings by lazy { CosmicSettings(CosmicApplication.context) }
        var updateSettings: Boolean = false

        private var dsCachedSet: HashMap<String, Any>? = null
        private fun updateAllValues() {
            if (!updateSettings)
                return
            dsCachedSet?.clear()

            val dsCached = mapOf(
                "dsdb_app_storage" to globalSettings.appStorage,
                "dsdb_gpu_turbo_mode" to globalSettings.gpuTurboMode,
                "dsdb_gpu_custom_driver" to globalSettings.customDriver,
                "dsdb_ee_mode" to globalSettings.eeMode,
                "dsdb_bios_path" to globalSettings.biosPath
            )
            dsCachedSet = HashMap(dsCached)
            updateSettings = false
        }
        @JvmStatic
        fun getDataStoreValue(config: String) : Any {
            if (!updateSettings)
                updateSettings = dsCachedSet == null
            updateAllValues()
            dsCachedSet?.let {
                if (!it.containsKey(config))
                    throw NotFoundException(config)
                return it[config]!!
            }
            return {}
        }
    }
}