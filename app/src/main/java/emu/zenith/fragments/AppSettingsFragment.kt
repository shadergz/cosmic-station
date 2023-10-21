package emu.zenith.fragments

import android.os.Bundle
import androidx.preference.PreferenceFragmentCompat
import emu.zenith.R

class AppSettingsFragment : PreferenceFragmentCompat() {
    override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
        addPreferencesFromResource(R.xml.app_preferences)
    }
}