package emu.cosmic.listeners

import android.content.Context
import android.content.Intent
import android.util.AttributeSet
import androidx.preference.Preference
import emu.cosmic.settings.BiosActivity
import emu.cosmic.settings.CustomDriverActivity

class ActivitySwapperListener @JvmOverloads
    constructor(context: Context, attrs: AttributeSet? = null, defStyleAttr : Int = androidx.preference.R.attr.preferenceStyle) :
    Preference(context, attrs, defStyleAttr) {
    private fun launchActivity() {
        var swapTo: Intent? = null
        when (title) {
            "BIOS Selector" -> {
                swapTo = Intent(context, BiosActivity::class.java)
            }
            "Custom Drivers" -> {
                swapTo = Intent(context, CustomDriverActivity::class.java)
            }
        }
        swapTo?.let {
            it.putExtra("Activity Provider", "App")
            context.startActivity(it)
        }
    }
    override fun onClick() = launchActivity()
}