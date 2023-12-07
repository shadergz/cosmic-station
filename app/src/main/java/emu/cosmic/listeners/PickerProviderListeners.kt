package emu.cosmic.listeners

import android.content.Context
import android.net.Uri
import android.os.Environment
import android.util.AttributeSet
import androidx.preference.Preference
import emu.cosmic.SettingsActivity
import emu.cosmic.data.CosmicSettings
import emu.cosmic.helpers.BiosHelper
import emu.cosmic.helpers.DriverHelper

class FolderPickerListener @JvmOverloads
    constructor(context: Context, attrs: AttributeSet? = null, defStyleAttr : Int = androidx.preference.R.attr.preferenceStyle) :
    Preference(context, attrs, defStyleAttr) {

    private val settings = CosmicSettings.globalSettings
    private val picker = SettingsActivity.filePicker
    private val callback = { pair: Pair<Uri?, String> ->
        if (pair.second == "App Storage Directory")
            modifyAppDir(pair.first!!)
    }

    private fun modifyAppDir(dirPath: Uri) {
        settings.appStorage = context.pathSolver(dirPath)
        // Resetting all static data resources loaded from the last storage directory
        BiosHelper.toDefault()
        DriverHelper.toDefault()
    }

    override fun onClick() {
        SettingsActivity.threatPickerEvent = callback
        picker?.launch(title)
    }
}

@Suppress("UnusedReceiverParameter")
fun Context.pathSolver(original: Uri, with: String? = null): String {
    val schemes = arrayOf("/document/primary", "/document", "/tree/primary")
    var realFile = ""
    val target = original.path!!
    for (scheme in schemes) {
        if (!target.startsWith(scheme))
            continue
        realFile = target.substringAfter(scheme).replace(':', '/')
        break
    }
    if (with != null)
        return "$with${realFile}"
    val canonical = if (target.contains("primary")) {
        Environment.getExternalStorageDirectory().path
    } else {
        Environment.getStorageDirectory().path
    }
    return "$canonical${realFile}"
}
