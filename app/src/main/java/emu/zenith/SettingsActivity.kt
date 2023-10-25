package emu.zenith

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContract
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import androidx.preference.Preference
import androidx.preference.PreferenceFragmentCompat
import emu.zenith.data.ZenithSettings
import emu.zenith.databinding.SettingsActivityBinding
import emu.zenith.fragments.SettingsFragment

class SettingsActivity : AppCompatActivity(), PreferenceFragmentCompat.OnPreferenceStartFragmentCallback {
    private val binding by lazy {
        SettingsActivityBinding.inflate(layoutInflater)
    }
    private val preference by lazy { SettingsFragment() }

    companion object {
        var filePicker: ActivityResultLauncher<CharSequence>? = null
        var threatPickerEvent: ((Pair<Uri?, String>) -> Unit)? = null
    }
    private val settings = ZenithSettings.globalSettings

    inner class DirectoryPickerContract : ActivityResultContract<CharSequence, Pair<Uri?, String>>() {
        private lateinit var holderName: String
        override fun createIntent(context: Context, input: CharSequence): Intent =
            Intent(Intent.ACTION_OPEN_DOCUMENT_TREE).apply {
                putExtra("EXTRA_INITIAL_URI", settings.appStorage)
                holderName = "$input"
            }
        override fun parseResult(resultCode: Int, intent: Intent?): Pair<Uri?, String> {
            return when (resultCode) {
                Activity.RESULT_OK -> Pair(intent?.data, holderName)
                else -> Pair(null, holderName)
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        filePicker = registerForActivityResult(DirectoryPickerContract()) { pair ->
            if (pair.first == null)
                return@registerForActivityResult

            contentResolver?.takePersistableUriPermission(
                pair.first!!, Intent.FLAG_GRANT_READ_URI_PERMISSION)
            threatPickerEvent?.invoke(pair)
        }

        // Add the back button, so the user can return to the main screen
        WindowCompat.setDecorFitsSystemWindows(window, false)
        binding.appToolBar.apply {
            title = resources.getString(R.string.toolbar_settings)
        }

        setSupportActionBar(binding.appToolBar)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        supportFragmentManager.beginTransaction()
            .replace(R.id.preferencesContainer, preference)
            .commit()
    }

    override fun onPreferenceStartFragment(caller: PreferenceFragmentCompat, pref: Preference): Boolean {
        val fragment = supportFragmentManager.fragmentFactory.instantiate(classLoader, pref.fragment!!)
        fragment.arguments = pref.extras

        // Replace the existing Fragment with the new Fragment
        when (pref.fragment) {
            "emu.zenith.fragments.GlobalSettingsFragment" -> {
                supportActionBar?.title = "Global Settings"
            }
            "emu.zenith.fragments.GraphicsSettingsFragment" -> {
                supportActionBar?.title = "Graphics Settings"
            }
        }
        supportFragmentManager.beginTransaction()
            .replace(R.id.preferencesContainer, fragment)
            .addToBackStack(null)
            .commit()
        return true
    }
}