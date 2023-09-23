package emu.zenith.data
import android.content.Context
import androidx.datastore.preferences.core.Preferences
import androidx.datastore.preferences.core.stringPreferencesKey
import emu.zenith.R
import emu.zenith.appExternalDir

enum class SettingsKeys(val dsDbPrefer: Int) {
    RootDirectory(R.string.datastore_root_dir)
}

class SettingContainer<T>(context: Context, key: SettingsKeys) {
    val defaultValue: T
    val preferKey: Preferences.Key<T>

    private var keyValue: String
    val containerContext = context

    init {
        keyValue = context.getString(key.dsDbPrefer)
        @Suppress("UNCHECKED_CAST")
        preferKey = stringPreferencesKey(keyValue) as Preferences.Key<T>

        when (key) {
            SettingsKeys.RootDirectory -> {
                @Suppress("UNCHECKED_CAST")
                defaultValue = context.appExternalDir.path as T
            }
        }
    }
}
