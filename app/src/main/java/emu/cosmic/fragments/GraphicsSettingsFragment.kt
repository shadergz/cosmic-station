package emu.cosmic.fragments

import android.os.Bundle
import androidx.preference.PreferenceFragmentCompat
import androidx.preference.TwoStatePreference
import emu.cosmic.R
import emu.cosmic.data.CosmicSettings
import emu.cosmic.helpers.DriverHelper

class GraphicsSettingsFragment : PreferenceFragmentCompat() {
    override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
        addPreferencesFromResource(R.xml.graphics_settings)
        activity?.actionBar?.title = "Graphics Settings"

        val gpuTurboMode = findPreference<TwoStatePreference>("gpuTurboMode")
        val settings = CosmicSettings.globalSettings

        gpuTurboMode?.setOnPreferenceChangeListener  { _, value ->
            if (value is Boolean) {
                DriverHelper.switchTurboMode(value)
                settings.gpuTurboMode = value
            }
            true
        }
        gpuTurboMode?.isChecked = settings.gpuTurboMode
    }
}