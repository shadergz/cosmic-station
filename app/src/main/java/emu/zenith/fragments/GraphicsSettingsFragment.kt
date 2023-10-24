package emu.zenith.fragments

import android.os.Bundle
import androidx.preference.PreferenceFragmentCompat
import androidx.preference.TwoStatePreference
import emu.zenith.R
import emu.zenith.data.ZenithSettings

class GraphicsSettingsFragment : PreferenceFragmentCompat() {
    override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
        addPreferencesFromResource(R.xml.graphics_settings)

        val gpuTurboMode = findPreference<TwoStatePreference>("gpuTurboMode")
        val settings = ZenithSettings.globalSettings

        gpuTurboMode?.setOnPreferenceChangeListener  { _, value ->
            if (value is Boolean) {
                gpuTurboMode(value)
                settings.gpuTurboMode = value
            }
            true
        }
        gpuTurboMode?.isChecked = settings.gpuTurboMode
    }
    private external fun gpuTurboMode(enable: Boolean)
}