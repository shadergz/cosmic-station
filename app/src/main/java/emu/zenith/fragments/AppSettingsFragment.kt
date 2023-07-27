package emu.zenith.fragments

import android.os.Bundle
import android.util.Log
import androidx.preference.Preference
import androidx.preference.PreferenceFragmentCompat
import emu.zenith.R

class AppSettingsFragment : PreferenceFragmentCompat() {
    override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
        addPreferencesFromResource(R.xml.app_preferences)

        findPreference<Preference>("storageRootKey")?.setOnPreferenceClickListener {
            Log.d("Preference Listener", "Storage root key")
            true
        }
    }

}