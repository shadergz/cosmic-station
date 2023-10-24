package emu.zenith.fragments

import android.os.Bundle
import androidx.preference.PreferenceFragmentCompat
import androidx.preference.TwoStatePreference
import emu.zenith.R
import emu.zenith.data.ZenithSettings
import emu.zenith.helpers.GpuHelper

class GraphicsSettingsFragment : PreferenceFragmentCompat() {
    override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
        addPreferencesFromResource(R.xml.graphics_settings)

        val gpuTurboMode = findPreference<TwoStatePreference>("gpuTurboMode")
        val settings = ZenithSettings.globalSettings

        gpuTurboMode?.setOnPreferenceChangeListener  { _, value ->
            if (value is Boolean) {
                GpuHelper.switchTurboMode(value)
                settings.gpuTurboMode = value
            }
            true
        }
        gpuTurboMode?.isChecked = settings.gpuTurboMode
    }
}