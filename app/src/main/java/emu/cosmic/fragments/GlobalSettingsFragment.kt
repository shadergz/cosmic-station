package emu.cosmic.fragments

import android.os.Bundle
import androidx.preference.PreferenceFragmentCompat
import emu.cosmic.R

class GlobalSettingsFragment : PreferenceFragmentCompat() {
    override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
        addPreferencesFromResource(R.xml.global_settings)
        activity?.actionBar?.title = "Global Settings"
    }
}