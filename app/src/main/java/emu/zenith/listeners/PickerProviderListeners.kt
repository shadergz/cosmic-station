package emu.zenith.listeners

import android.content.Context
import android.net.Uri
import android.os.Environment
import android.util.AttributeSet
import androidx.preference.Preference
import emu.zenith.SettingsActivity
import emu.zenith.data.ZenithSettings
import emu.zenith.helpers.BiosHelperModel

class FolderPickerListener @JvmOverloads
    constructor(context: Context, attrs: AttributeSet? = null, defStyleAttr : Int = androidx.preference.R.attr.preferenceStyle) :
    Preference(context, attrs, defStyleAttr) {

    private val settings = ZenithSettings.globalSettings
    private val picker = SettingsActivity.filePicker
    private val callback = { pair: Pair<Uri?, String> ->
        if (pair.second == "App Storage Directory")
            modifyAppDir(pair.first?.path!!)
    }

    private fun treePathSolver(original: String): String {
        val primaryStorage = Environment.getExternalStorageDirectory().path
        if (original.startsWith("/tree/primary:")) {
            val remainingPath = original.substring("/tree/primary:".length)
            return "$primaryStorage/$remainingPath"
        }
        return original
    }

    private fun modifyAppDir(dirPath: String) {
        settings.appStorage = treePathSolver(dirPath)
        // Resetting all static data resources loaded from the last storage directory
        BiosHelperModel.toDefault()
    }

    override fun onClick() {
        SettingsActivity.threatPickerEvent = callback
        picker?.launch(title)
    }
}