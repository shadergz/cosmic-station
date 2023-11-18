package emu.cosmic.data
import android.content.Context
import android.os.Environment
import androidx.datastore.preferences.core.Preferences
import androidx.datastore.preferences.core.stringPreferencesKey
import emu.cosmic.R

enum class SettingsKeys(val dsdbKey: Int) {
    AppStorage(R.string.datastore_app_storage),
    GpuTurboMode(R.string.datastore_gpu_turbo_mode),
    CustomDriver(R.string.datastore_gpu_custom_driver),
    EEMode(R.string.datastore_ee_mode),
    BiosPath(R.string.datastore_bios_path)
}

@Suppress("unchecked_cast")
class SettingContainer<T>(context: Context, key: SettingsKeys) {
    val defaultValue: T
    val preferKey: Preferences.Key<T>

    private var keyValue: String
    val containerContext = context

    init {
        keyValue = context.getString(key.dsdbKey)
        preferKey = stringPreferencesKey(keyValue) as Preferences.Key<T>

        defaultValue = when (key) {
            SettingsKeys.AppStorage -> {
                val envDir = Environment.getExternalStorageDirectory()
                envDir.path as T
            }
            SettingsKeys.GpuTurboMode -> { false as T }
            SettingsKeys.CustomDriver -> { "libvulkan.so" as T }
            SettingsKeys.EEMode -> { 1 as T }
            SettingsKeys.BiosPath -> { "" as T }
        }
    }
}