package emu.zenith.listeners

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.net.Uri
import android.util.AttributeSet
import androidx.activity.ComponentActivity
import androidx.activity.result.contract.ActivityResultContract
import androidx.preference.Preference
import emu.zenith.data.ZenithSettings

class FolderPickerListener @JvmOverloads
    constructor(context: Context, attrs: AttributeSet? = null, defStyleAttr : Int = androidx.preference.R.attr.preferenceStyle) :
    Preference(context, attrs, defStyleAttr) {

    private val globalSettings = ZenithSettings.globalSettings

    private fun modifyRootDirectory(dirPath: String) {
        if (dirPath != globalSettings.rootDirectory) {
            globalSettings.rootDirectory = dirPath
        }
    }

    inner class DirectoryPickerContract : ActivityResultContract<CharSequence, Uri?>() {
        override fun createIntent(context: Context, input: CharSequence): Intent =
            Intent(Intent.ACTION_OPEN_DOCUMENT_TREE).apply {
                if (input == "Modify App Storage")
                    putExtra("EXTRA_INITIAL_URI", globalSettings.rootDirectory)
            }
        override fun parseResult(resultCode: Int, intent: Intent?): Uri? {
            return when (resultCode) {
                Activity.RESULT_OK -> intent?.data
                else -> null
            }
        }
    }

    private val filePicker = (context as ComponentActivity).registerForActivityResult(
        DirectoryPickerContract()) {
        it?.let { uri ->
            context.contentResolver.takePersistableUriPermission(uri,
                Intent.FLAG_GRANT_READ_URI_PERMISSION)
            if (title == "Modify App Storage")
                modifyRootDirectory(uri.toString())
        }
    }
    override fun onClick() = filePicker.launch(title)
}