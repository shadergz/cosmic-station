package emu.zenith.listeners

import android.content.Context
import android.content.Intent
import android.util.AttributeSet
import androidx.preference.Preference
import emu.zenith.settings.KernelActivity

class ActivitySwapperListener @JvmOverloads
    constructor(context: Context, attrs: AttributeSet? = null, defStyleAttr : Int = androidx.preference.R.attr.preferenceStyle) :
    Preference(context, attrs, defStyleAttr) {

    private fun launchActivity() {
        var swapTo: Intent? = null
        when (title) {
            "Kernel Selector" -> {
                swapTo = Intent(context, KernelActivity::class.java)
            }
        }
        swapTo?.let {
            it.putExtra("Activity Provider", "Zenith")
            context.startActivity(it)
        }
    }
    override fun onClick() = launchActivity()
}