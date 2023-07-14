package emu.zenith

import android.app.Application
import com.google.android.material.color.DynamicColors

class ZenithApplication : Application() {
    override fun onCreate() {
        super.onCreate()
        DynamicColors.applyToActivitiesIfAvailable(this)

        System.loadLibrary("zenith")
    }
}