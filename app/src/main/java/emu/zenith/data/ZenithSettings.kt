package emu.zenith.data

import android.content.Context
import androidx.datastore.preferences.core.stringPreferencesKey
import emu.zenith.ZenithApplication
import emu.zenith.appExternalDir

@Suppress("Unused")
class ZenithSettings private constructor(context: Context) {

    private val rdKey = context.getString(SettingsKeys.RootDirectory.dsDbPrefer)
    private val rdDefault = context.appExternalDir.absolutePath
    var rootDirectory by DelegateDataStore<String>(context = context, key = stringPreferencesKey(rdKey), defaultValue = rdDefault)

    companion object {
        // Creating a static object to store all our configurations; this object will reside in the
        // global heap memory (Accessible to JNI)
        val globalSettings by lazy { ZenithSettings(ZenithApplication.context) }
    }
}
