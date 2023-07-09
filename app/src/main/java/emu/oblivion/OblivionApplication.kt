package emu.oblivion

import android.app.Application
import com.google.android.material.color.DynamicColors

class OblivionApplication : Application() {
    override fun onCreate() {
        super.onCreate()
        DynamicColors.applyToActivitiesIfAvailable(this)

        System.loadLibrary("oblivion")
    }
}