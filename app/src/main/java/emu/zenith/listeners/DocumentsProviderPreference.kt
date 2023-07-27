package emu.zenith.listeners

import android.content.Context
import android.content.Intent
import android.util.AttributeSet
import androidx.preference.Preference

class DocumentsProviderPreference @JvmOverloads
    constructor(context: Context, attrs: AttributeSet? = null, defStyleAttr : Int = androidx.preference.R.attr.preferenceStyle) :
    Preference(context, attrs, defStyleAttr) {

    private fun launchIntent() {
        val openIntent = Intent(Intent.ACTION_OPEN_DOCUMENT_TREE)
        context.startActivity(openIntent)
    }

    override fun onClick() {
        super.onClick()
        launchIntent()
    }

}