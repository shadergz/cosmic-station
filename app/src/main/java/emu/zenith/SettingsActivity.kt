package emu.zenith

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import emu.zenith.databinding.SettingsActivityBinding
import emu.zenith.fragments.AppSettingsFragment

class SettingsActivity : AppCompatActivity() {
    private val binding by lazy {
        SettingsActivityBinding.inflate(layoutInflater)
    }
    private val preference by lazy {
        AppSettingsFragment()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        // Add the back button, so the user can return to the main screen
        WindowCompat.setDecorFitsSystemWindows(window, false)
        binding.appToolBar.headerToolBar.apply {
            title = resources.getString(R.string.toolbar_settings)
        }

        setSupportActionBar(binding.appToolBar.headerToolBar)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        supportFragmentManager.beginTransaction()
            .replace(R.id.preferencesContainer, preference)
            .commit()

    }

}